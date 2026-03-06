#include "pch.hpp"
#include "DifferentGamemode.hpp"

#include "GameStateManager.hpp" 
#include "GameStateList.hpp"    
#include "Audio.hpp"           
#include "Utils.hpp"
#include <cmath>

int gMazeRewardsEarned = 0;

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
// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
static AEVec2 TileCenter(int r, int c)
{
    AEVec2 p;
    p.x = kWorldLeft + (c + 0.5f) * kTileSize;
    p.y = kWorldBottom + (kMazeRows - 1 - r + 0.5f) * kTileSize; // flip row
    return p;
}

static bool IsWallRC(int r, int c)
{
    if (r < 0 || r >= kMazeRows || c < 0 || c >= kMazeCols) return true;
    return kMaze[r][c] == '#';
}

static void FindStartAndGoal()
{
    bool foundS = false;
    bool foundG = false;

    for (int r = 0; r < kMazeRows; ++r)
    {
        for (int c = 0; c < kMazeCols; ++c)
        {
            const char cell = kMaze[r][c];

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
        gGoalPos = TileCenter(kMazeRows - 2, kMazeCols - 2);
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
    const int approxR = (kMazeRows - 1) - approxRFromBottom;

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
    createUnitSquare(&unit_square);
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
    AEGfxSetBackgroundColor(0.2f, 0.0f, 0.2f);

    FindStartAndGoal();

    gPlayerVel = { 0.0f, 0.0f };

    ResolvePlayerVsWalls();

    AEGfxSetCamPosition(gPlayerPos.x, gPlayerPos.y);
}

void GameState_MazeUpdate()
{
    const float dt = (float)AEFrameRateControllerGetFrameTime();

    if (AEInputCheckTriggered(AEVK_ESCAPE))
    {
        PlayClick();
        next = GAME_STATE_MENU;
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
        PlayClick();
        next = GAME_STATE_MENU; 
    }
}

void GameState_MazeDraw()
{

    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0.0f, 0.0f);

    AEGfxSetCamPosition(gPlayerPos.x, gPlayerPos.y);

    for (int r = 0; r < kMazeRows; ++r)
    {
        for (int c = 0; c < kMazeCols; ++c)
        {
            const char t = kMaze[r][c];
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

    drawQuad(unit_square,
        gPlayerPos.x, gPlayerPos.y,
        kPlayerRadius * 2.0f, kPlayerRadius * 2.0f,
        1.0f, 0.2f, 0.2f, 1.0f);
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
}