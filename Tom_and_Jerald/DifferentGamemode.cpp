#include "pch.hpp"
#include "DifferentGamemode.hpp"
#include "Animation.hpp"
#include "ImgFontInit.hpp"
#include "GameStateManager.hpp" 
#include "GameStateList.hpp"    
#include "Audio.hpp"           
#include "Utils.hpp"
#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

int gMazeRewardsEarned = 0;
bool gMazeFromPlaying = false;
bool gMazeCompleted = false;
bool gMazeSuccess = false;

// ----------------------------------------------------------------------------
// Simple maze representation
// '#' = wall
// '.' = floor
// 'S' = start
// 'G' = goal
// ----------------------------------------------------------------------------
static const char* kMaze[] = { "####################", 
                               "#S....#.......#....#", 
                               "#.##.#######.#.###.#", 
                               "#.......#..#.#..##.#", 
                               "####.#.###.#.##.##.#", 
                               "#....#...#.#....##.#", 
                               "#.####.#.#.#######.#", 
                               "#......#.#......##.#", 
                               "#.######.##.###.##.#", 
                               "#.............#...G#",  
                               "####################" 
};
static constexpr int   kMazeRows = (int)(sizeof(kMaze) / sizeof(kMaze[0]));
static constexpr int   kMazeCols = 20;
static constexpr float kTileSize = 48.0f;

static constexpr float kWorldLeft = 80.0f;
static constexpr float kWorldBottom = 60.0f;

static AEVec2 gPlayerPos{ 0.0f, 0.0f };
static AEVec2 gPlayerVel{ 0.0f, 0.0f };
static constexpr float kPlayerRadius = 16.0f;
static constexpr float kPlayerSpeed = 260.0f;

static AEVec2 gGoalPos{ 0.0f, 0.0f };
static constexpr float kGoalRadius = 18.0f;

static AEGfxVertexList* gMeshQuad = nullptr;
static AEGfxVertexList* gMeshCircle = nullptr;
static AEGfxVertexList* unit_square = nullptr;
static AEGfxVertexList* maze_player_mesh = nullptr;
// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
static const char* gMazeFiles[] =
{
    "Assets/data/maze1.txt",
    "Assets/data/maze2.txt",
    "Assets/data/maze3.txt",
    "Assets/data/maze4.txt",
    "Assets/data/maze5.txt"
};

static constexpr int gMazeFileCount = sizeof(gMazeFiles) / sizeof(gMazeFiles[0]);
static int gLastMazeIndex = -1;
static std::vector<std::string> gLoadedMaze;
static bool gUseLoadedMaze = false;
static int GetMazeRows();
static int GetMazeCols();
static char GetMazeCell(int r, int c);


static AEVec2 TileCenter(int r, int c)
{
    AEVec2 p;
    p.x = kWorldLeft + (c + 0.5f) * kTileSize;
    p.y = kWorldBottom + (GetMazeRows() - 1 - r + 0.5f) * kTileSize; // flip row
    return p;
}

static int GetMazeRows()
{
    if (gUseLoadedMaze)
        return (int)gLoadedMaze.size();

    return kMazeRows;
}

static int GetMazeCols()
{
    if (gUseLoadedMaze && !gLoadedMaze.empty())
        return (int)gLoadedMaze[0].size();

    return kMazeCols;
}

static char GetMazeCell(int r, int c)
{
    if (gUseLoadedMaze)
        return gLoadedMaze[r][c];

    return kMaze[r][c];
}

static bool LoadMazeFromFile(const char* fileName)
{
    std::ifstream file(fileName);
    if (!file.is_open())
        return false;

    gLoadedMaze.clear();
    std::string line;

    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (!line.empty())
            gLoadedMaze.push_back(line);
    }

    file.close();

    if (gLoadedMaze.empty())
        return false;

    const int cols = (int)gLoadedMaze[0].size();

    for (size_t i = 1; i < gLoadedMaze.size(); ++i)
    {
        if ((int)gLoadedMaze[i].size() != cols)
        {
            gLoadedMaze.clear();
            return false;
        }
    }

    gUseLoadedMaze = true;
    return true;
}

static void LoadRandomMazeFile()
{
    gUseLoadedMaze = false;
    gLoadedMaze.clear();

    if (!gMazeFileCount) return;

    int index = 0;

    if constexpr (gMazeFileCount == 1)
    {
        index = 0;
    }
    else
    {
        do
        {
            index = rand() % gMazeFileCount;
        } while (index == gLastMazeIndex);
    }

    if (LoadMazeFromFile(gMazeFiles[index]))
        gLastMazeIndex = index;
}

static bool IsWallRC(int r, int c)
{
    if (r < 0 || r >= GetMazeRows() || c < 0 || c >= GetMazeCols())
        return true;

    return GetMazeCell(r, c) == '#';
}

static void FindStartAndGoal()
{
    bool foundS = false;
    bool foundG = false;

    for (int r = 0; r < GetMazeRows(); ++r)
    {
        for (int c = 0; c < GetMazeCols(); ++c)
        {
            const char cell = GetMazeCell(r, c);

            if (cell == 'S')
            {
                gPlayerPos = TileCenter(r, c);
                foundS = true;
            }
            else if (cell == 'G')
            {
                gGoalPos = TileCenter(r, c);
                foundG = true;
            }
        }
    }

    if (!foundS)
        gPlayerPos = TileCenter(1, 1);

    if (!foundG)
        gGoalPos = TileCenter(GetMazeRows() - 2, GetMazeCols() - 2);
}

static float Clamp(float v, float lo, float hi)
{
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

static void ResolveCircleAABB(AEVec2& circlePos, float radius,
    const AEVec2& boxCenter, float halfW, float halfH)
{
    const float minX = boxCenter.x - halfW;
    const float maxX = boxCenter.x + halfW;
    const float minY = boxCenter.y - halfH;
    const float maxY = boxCenter.y + halfH;

    const float closestX = Clamp(circlePos.x, minX, maxX);
    const float closestY = Clamp(circlePos.y, minY, maxY);

    const float dx = circlePos.x - closestX;
    const float dy = circlePos.y - closestY;

    const float distSq = dx * dx + dy * dy;
    const float rSq = radius * radius;

    if (distSq >= rSq) return;

    if (distSq < 0.0001f)
    {
        const float leftPen = (circlePos.x - minX);
        const float rightPen = (maxX - circlePos.x);
        const float botPen = (circlePos.y - minY);
        const float topPen = (maxY - circlePos.y);

        float minPen = leftPen;
        AEVec2 push{ -1.0f, 0.0f };

        if (rightPen < minPen) { minPen = rightPen; push = { 1.0f, 0.0f }; }
        if (botPen < minPen) { minPen = botPen;   push = { 0.0f,-1.0f }; }
        if (topPen < minPen) { minPen = topPen;   push = { 0.0f, 1.0f }; }

        circlePos.x += push.x * (radius - minPen);
        circlePos.y += push.y * (radius - minPen);
        return;
    }

    const float dist = sqrtf(distSq);
    const float pen = radius - dist;

    const float nx = dx / dist;
    const float ny = dy / dist;

    circlePos.x += nx * pen;
    circlePos.y += ny * pen;
}

static void ResolvePlayerVsWalls()
{
    const float relX = (gPlayerPos.x - kWorldLeft) / kTileSize;
    const float relY = (gPlayerPos.y - kWorldBottom) / kTileSize;

    const int approxC = (int)relX;
    const int approxRFromBottom = (int)relY;
    const int approxR = (GetMazeRows() - 1) - approxRFromBottom;

    for (int dr = -2; dr <= 2; ++dr)
    {
        for (int dc = -2; dc <= 2; ++dc)
        {
            const int r = approxR + dr;
            const int c = approxC + dc;
            if (!IsWallRC(r, c)) continue;

            const AEVec2 center = TileCenter(r, c);
            ResolveCircleAABB(gPlayerPos, kPlayerRadius, center, kTileSize * 0.5f, kTileSize * 0.5f);
        }
    }
}

static bool PlayerReachedGoal()
{
    const float dx = gPlayerPos.x - gGoalPos.x;
    const float dy = gPlayerPos.y - gGoalPos.y;
    const float r = kPlayerRadius + kGoalRadius;
    return (dx * dx + dy * dy) <= (r * r);
}

void GameState_MazeLoad()
{
    srand((unsigned int)time(nullptr));
    ASSETS::Init_Images();
    ASSETS::Init_Font();
    createUnitSquare(&unit_square);
    ANIMATION::player.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::player.Clip_Select(0, 0, 2, 10.0f);
    createUnitSquare(&maze_player_mesh, 0.25f, 0.25f);
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

    gMeshQuad = AEGfxMeshEnd();

    AEGfxMeshStart();
    const int segments = 28;
    for (int i = 0; i < segments; ++i)
    {
        const float a0 = (2.0f * PI * i) / segments;
        const float a1 = (2.0f * PI * (i + 1)) / segments;

        const float x0 = cosf(a0) * 0.5f;
        const float y0 = sinf(a0) * 0.5f;
        const float x1 = cosf(a1) * 0.5f;
        const float y1 = sinf(a1) * 0.5f;

        AEGfxTriAdd(0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,
            x0, y0, 0xFFFFFFFF, 0.0f, 1.0f,
            x1, y1, 0xFFFFFFFF, 1.0f, 1.0f);
    }
    gMeshCircle = AEGfxMeshEnd();
}

void GameState_MazeInit()
{
    gMazeCompleted = false;

    gMazeSuccess = false;

    AEGfxSetBackgroundColor(0.2f, 0.0f, 0.2f);

    LoadRandomMazeFile();

    FindStartAndGoal();

    gPlayerVel = { 0.0f, 0.0f };

    ResolvePlayerVsWalls();

    AEGfxSetCamPosition(gPlayerPos.x, gPlayerPos.y);
}

void GameState_MazeUpdate()
{
    const float dt = (float)AEFrameRateControllerGetFrameTime();
    ANIMATION::player.Anim_Update(dt);

    if (AEInputCheckTriggered(AEVK_ESCAPE))
    {
        gMazeCompleted = true;
        gMazeSuccess = false;
        next = gMazeFromPlaying ? GAME_STATE_PLAYING : GAME_STATE_MENU;
        return;
    }

    float ix = 0.0f, iy = 0.0f;

    if (AEInputCheckCurr(AEVK_LEFT) || AEInputCheckCurr('A')) ix -= 1.0f;
    if (AEInputCheckCurr(AEVK_RIGHT) || AEInputCheckCurr('D')) ix += 1.0f;
    if (AEInputCheckCurr(AEVK_UP) || AEInputCheckCurr('W')) iy += 1.0f;
    if (AEInputCheckCurr(AEVK_DOWN) || AEInputCheckCurr('S')) iy -= 1.0f;

    const float lenSq = ix * ix + iy * iy;
    if (lenSq > 0.0001f)
    {
        const float invLen = 1.0f / sqrtf(lenSq);
        ix *= invLen;
        iy *= invLen;
    }

    gPlayerVel.x = ix * kPlayerSpeed;
    gPlayerVel.y = iy * kPlayerSpeed;

    gPlayerPos.x += gPlayerVel.x * dt;
    gPlayerPos.y += gPlayerVel.y * dt;

    ResolvePlayerVsWalls();
    AEGfxSetCamPosition(gPlayerPos.x, gPlayerPos.y);
    if (PlayerReachedGoal())
    {
        gMazeRewardsEarned += 1;
        gMazeCompleted = true;
        gMazeSuccess = true;
        next = gMazeFromPlaying ? GAME_STATE_PLAYING : GAME_STATE_MENU;
    }
}

void GameState_MazeDraw()
{

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0.0f, 0.0f);

    AEGfxSetCamPosition(gPlayerPos.x, gPlayerPos.y);

    for (int r = 0; r < GetMazeRows(); ++r)
    {
        for (int c = 0; c < GetMazeCols(); ++c)
        {
            const char t = GetMazeCell(r, c);
            const AEVec2 p = TileCenter(r, c);

            if (t == '#')
            {
                drawQuad(unit_square, p.x, p.y, kTileSize, kTileSize, 1.0f, 1.0f, 1.0f, 1.0f);
            }
            else
            {
                drawQuad(unit_square, p.x, p.y, kTileSize, kTileSize, 0.25f, 0.25f, 0.25f, 1.0f);

                if (t == 'G')
                    drawQuad(unit_square, p.x, p.y, kTileSize, kTileSize, 0.1f, 1.0f, 0.1f, 1.0f);
                else if (t == 'S')
                    drawQuad(unit_square, p.x, p.y, kTileSize, kTileSize, 0.2f, 0.6f, 1.0f, 1.0f);
            }
        }
    }

    ANIMATION::player.Anim_Draw(ASSETS::playerAssets);

    drawQuad(maze_player_mesh,
        gPlayerPos.x,
        gPlayerPos.y,
        kPlayerRadius * 2.5f,
        kPlayerRadius * 2.5f,
        1.0f, 1.0f, 1.0f, 1.0f);
}

void GameState_MazeFree()
{

}

void GameState_MazeUnload()
{
    if (gMeshQuad) { AEGfxMeshFree(gMeshQuad);   gMeshQuad = nullptr; }
    if (gMeshCircle) { AEGfxMeshFree(gMeshCircle); gMeshCircle = nullptr; }
    if (unit_square)
    {
        AEGfxMeshFree(unit_square);
        unit_square = nullptr;
    }

    ANIMATION::player.Anim_Draw(ASSETS::playerAssets);

    drawQuad(maze_player_mesh,
        gPlayerPos.x,
        gPlayerPos.y,
        kPlayerRadius * 2.5f,
        kPlayerRadius * 2.5f,
        1.0f, 1.0f, 1.0f, 1.0f);

    ASSETS::Unload_Images();
    ASSETS::Unload_Font();

    gLoadedMaze.clear();
    gUseLoadedMaze = false;

}