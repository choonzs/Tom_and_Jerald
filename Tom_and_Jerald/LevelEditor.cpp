#include "pch.hpp"
#include "LevelEditor.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Obstacle.hpp"
#include "ImgFontInit.hpp"
#include "LevelTile.hpp"
#include "Audio.hpp"
#include "Animation.hpp"

namespace {
    int VIEW_COLS = 0;
    const int VIEW_ROWS = 36;
    f32 TILE_SIZE = 0.0f;
    f32 uiWidth = 240.0f;

	// Map Data: 2D grid representing the level layout. Each int corresponds to a TileType.
    //std::vector<std::vector<int>> mapData;


    // New Map Data
	std::vector<std::vector<LevelTile>> mapTiles;


    int viewOffsetX = 0;
    f32 scrollTimer = 0.0f; // Controls smooth scrolling speed

    // Tool Selection State
    bool isDragging = false;
	ObstacleType currentTool = Asteroid;  // 1 = Asteroid, 2 = Spike
    f32 mouseX, mouseY;

	// --- UI Values ---
    std::pair<ObstacleSpeed, ObstacleSpeed> obstacle_speed( ObstacleSpeed::Normal,ObstacleSpeed::Normal );
    std::pair<f32, f32> velocity{};

    // --- Obstacle Scale ---
    ObstacleScale obstacle_size{ ObstacleScale::Normal };
    f32 obstacle_scale{}; //by default

    // Textures & Meshes
    AEGfxVertexList* meshWhite = nullptr;
    AEGfxVertexList* meshUIBg = nullptr;
    AEGfxVertexList* meshSlotBg = nullptr;
    AEGfxVertexList* meshSlotBorder = nullptr;
    AEGfxVertexList* meshGrid = nullptr;
    // Icon mesh: UV covers one cell of the 4x4 spritesheet (0 to 0.25)
    AEGfxVertexList* meshIcon = nullptr;

    // Spritesheet UV offsets for SS_BackgroundAssets.png (4x4 grid, each cell = 0.25)
    // Asteroid: row 2, col 0  -> uv offset (0.00, 0.50)
    // Spike:    row 1, col 1  -> uv offset (0.25, 0.25)
    constexpr f32 UV_ASTEROID_X = 0.00f;
    constexpr f32 UV_ASTEROID_Y = 0.50f;
    constexpr f32 UV_SPIKE_X    = 0.25f;
    constexpr f32 UV_SPIKE_Y    = 0.25f;
    constexpr f32 UV_CELL       = 0.25f;

    AEGfxVertexList* CreateSquareMesh(u32 color) {
        AEGfxMeshStart();
        AEGfxTriAdd(-0.5f, -0.5f, color, 0.0f, 1.0f, 0.5f, -0.5f, color, 1.0f, 1.0f, -0.5f, 0.5f, color, 0.0f, 0.0f);
        AEGfxTriAdd(0.5f, -0.5f, color, 1.0f, 1.0f, 0.5f, 0.5f, color, 1.0f, 0.0f, -0.5f, 0.5f, color, 0.0f, 0.0f);
        return AEGfxMeshEnd();
    }
}

void LevelEditor_Load() {
    ASSETS::Init_Font();
    ASSETS::Init_Images();
}

void LevelEditor_Initialize() {
    AEInputShowCursor(true);
    // Start with 100 columns. It will grow infinitely as you scroll right!
    //mapData.assign(VIEW_ROWS, std::vector<int>(100, 0));
	mapTiles.assign(VIEW_ROWS, std::vector<LevelTile>(100, {}));
    viewOffsetX = 0;
    isDragging = false;
    currentTool = Asteroid;

    AEGfxSetCamPosition(0.0f, 0.0f);
    AEGfxSetBackgroundColor(0.15f, 0.15f, 0.15f);

    meshWhite = CreateSquareMesh(0xFFFFFFFF);
    meshUIBg = CreateSquareMesh(0xFF2A2D34);
    meshSlotBg = CreateSquareMesh(0xFF3E434C);
    meshSlotBorder = CreateSquareMesh(0xFFF9A03F);
    meshGrid = CreateSquareMesh(0x1AFFFFFF);

    // Icon mesh with UV covering a single 4x4 spritesheet cell (0 to 0.25)
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, UV_CELL,
                 0.5f, -0.5f, 0xFFFFFFFF, UV_CELL, UV_CELL,
                -0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    AEGfxTriAdd( 0.5f, -0.5f, 0xFFFFFFFF, UV_CELL, UV_CELL,
                 0.5f,  0.5f, 0xFFFFFFFF, UV_CELL, 0.0f,
                -0.5f,  0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    meshIcon = AEGfxMeshEnd();

    // Animations
    ANIMATION::asteroid.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::asteroid.Clip_Select(2, 0, 2, 10.0f);
    ANIMATION::spike.ImportFromFile("Assets/AnimationData.txt");
    ANIMATION::spike.Clip_Select(1, 1, 1, 10.0f);
}

void LevelEditor_Update() {

    if (AEInputCheckTriggered(AEVK_ESCAPE)) { next = GAME_STATE_MENU; }

    f32 dt = (f32)AEFrameRateControllerGetFrameTime();
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();

    TILE_SIZE = (halfH * 2.0f) / VIEW_ROWS;
    VIEW_COLS = (int)(((halfW * 2.0f) - uiWidth) / TILE_SIZE) + 1;

    mouseX = (f32)mx - halfW;
    mouseY = -(f32)my + halfH;
    // --- OBJECT VELOCITY & SCALE ADJUSTMENTS ---
	if (AEInputCheckTriggered(AEVK_D)) ++obstacle_speed.first;
	if (AEInputCheckTriggered(AEVK_A)) --obstacle_speed.first;
	if (AEInputCheckTriggered(AEVK_W)) ++obstacle_speed.second;
	if (AEInputCheckTriggered(AEVK_S)) --obstacle_speed.second;
    velocity.first = GetObstacleSpeed(obstacle_speed.first);
    velocity.second = GetObstacleSpeed(obstacle_speed.second);

	// Scale adjustments using mouse scroll
    s32 scroll = 0;
    AEInputMouseWheelDelta(&scroll);

    if (scroll > 0)
    {
        ++obstacle_size;
    }
    else if (scroll < 0)
    {
        // Scrolled down 
        --obstacle_size;
    }
    /*if (AEInputCheckTriggered(AEVK_Z)) ++obstacle_size;
    if (AEInputCheckTriggered(AEVK_X)) --obstacle_size;*/
    // Update scale based on obstacle size enum
    obstacle_scale = GetObstacleSize(obstacle_size);
	/*if (AEInputCheckCurr(AEVK_Z)) obstacle_scale += .1f;
	if (AEInputCheckCurr(AEVK_X)) obstacle_scale -= .1f;*/


    // --- SMOOTH SCROLLING & INFINITE DYNAMIC GENERATION ---
    if (AEInputCheckCurr(AEVK_RIGHT)) {
        scrollTimer += dt;
        if (scrollTimer > 0.05f) { // Scroll speed limit
            viewOffsetX++;
            scrollTimer = 0.0f;

            // DYNAMIC GENERATION: Append a new empty column if we reach the map's boundary
            if (static_cast<size_t>(viewOffsetX + VIEW_COLS) >= mapTiles[0].size()) {
                for (int r = 0; r < VIEW_ROWS; ++r) {
                    LevelTile empty{};
                    mapTiles[r].push_back(empty); // 0 = Empty tile
                }
            }
        }
    }
    else if (AEInputCheckCurr(AEVK_LEFT) && viewOffsetX > 0) {
        scrollTimer += dt;
        if (scrollTimer > 0.05f) {
            viewOffsetX--;
            scrollTimer = 0.0f;
        }
    }
    else {
        scrollTimer = 0.05f;
    }

    // --- LEFT MOUSE LOGIC: UI Selection & Painting ---
    if (AEInputCheckCurr(AEVK_LBUTTON)) {
        if (mouseX < -halfW + uiWidth) {
            if (AEInputCheckTriggered(AEVK_LBUTTON)) {
                f32 slot1Y = halfH - 120.0f;
                f32 slot2Y = halfH - 260.0f;
                if (mouseY < slot1Y + 50.0f && mouseY > slot1Y - 50.0f) {
                    currentTool = Asteroid; isDragging = true; 
                } // SQUARE
                if (mouseY < slot2Y + 50.0f && mouseY > slot2Y - 50.0f) {
                    currentTool = Spike; isDragging = true; 
                } // SPIKE
            }
        }
        else {
            int gridX = (int)((mouseX - (-halfW + uiWidth)) / TILE_SIZE) + viewOffsetX;
            int gridY = (int)((mouseY + halfH) / TILE_SIZE);
            if (gridX >= 0 && gridX < mapTiles[0].size() && gridY >= 0 && gridY < VIEW_ROWS) {
                //mapData[gridY][gridX] = currentTool;
                LevelTile tmp(currentTool,
                    AEVec2({ -halfW + uiWidth + (gridX - viewOffsetX) * TILE_SIZE + TILE_SIZE / 2.0f, -halfH + gridY * TILE_SIZE + TILE_SIZE / 2.0f }),
                    AEVec2({ (TILE_SIZE / 2.0f), (TILE_SIZE / 2.0f)}),
                    AEVec2({ velocity.first, velocity.second }),
                    obstacle_scale
                );
                mapTiles[gridY][gridX] = tmp;
            }
        }
    }

    // --- RIGHT MOUSE LOGIC: Erasing ---
    if (AEInputCheckCurr(AEVK_RBUTTON)) {
        if (mouseX >= -halfW + uiWidth) {
            int gridX = (int)((mouseX - (-halfW + uiWidth)) / TILE_SIZE) + viewOffsetX;
            int gridY = (int)((mouseY + halfH) / TILE_SIZE);
            if (gridX >= 0 && gridX < mapTiles[0].size() && gridY >= 0 && gridY < VIEW_ROWS) {
                mapTiles[gridY][gridX] = LevelTile(); // Use default ctor to reset it back to non obstacle
                //mapTiles[gridY].erase(mapTiles[gridY].begin() + gridX);
            }
        }
    }

    if (AEInputCheckReleased(AEVK_LBUTTON)) isDragging = false;

    // --- PLAY TEST (P): export to slot 1 and immediately play ---
    if (AEInputCheckTriggered(AEVK_P)) {
        std::string previewFile = "MapLevel/ExportedLevel1.txt";
        std::ofstream outPrev(previewFile);
        if (outPrev.is_open()) {
            int currentMaxCols = static_cast<int>(mapTiles[0].size());
            outPrev << currentMaxCols << " " << VIEW_ROWS << "\n";
            for (int r = 0; r < VIEW_ROWS; ++r)
                for (int c = 0; c < currentMaxCols; ++c)
                    if (mapTiles[r][c].type != 0) outPrev << mapTiles[r][c];
            outPrev.close();
        }
        std::ofstream loadFile("MapLevel/LoadLevel.txt");
        if (loadFile.is_open()) { loadFile << 1; loadFile.close(); }
        next = GAME_STATE_CUSTOM_PLAY;
    }

    // --- EXPORTING ---
    if (AEInputCheckTriggered(AEVK_E)) {
		//TODO Get Max window position and set it as finish line for the level end
        int fileIndex = 1;
		std::string filename;

        // Shorten for easier readability
	    namespace fs = std::filesystem;

		// Dynamically find a filename that doesn't exist yet to avoid overwriting
        do {
            filename = "MapLevel/ExportedLevel" + std::to_string(fileIndex) + ".txt";
            ++fileIndex;
		} while (fs::exists(filename));

		// Opens output file stream and writes the current map data to the file
		std::ofstream outFile(filename); 
        if (outFile.is_open()) { 
            //int currentMaxCols = static_cast<int>(mapData[0].size());

            int currentMaxCols = static_cast<int>(mapTiles[0].size());
            outFile << currentMaxCols << " " << VIEW_ROWS << "\n";
            for (int r = 0; r < VIEW_ROWS; ++r) {
                for (int c = 0; c < currentMaxCols; ++c) {
                    // Temp fix to optimise process
                    if (mapTiles[r][c].type == 0) continue; // Dont save non-obstacle data
                    //outFile << mapData[r][c] << " ";
                    //
                    outFile << mapTiles[r][c];
                    /*outFile << mapTiles[r][c].type << " " << mapTiles[r][c].pos.x << " " << mapTiles[r][c].pos.y << " "
                            << mapTiles[r][c].half_size.x << " " << mapTiles[r][c].half_size.y << " "
						<< velocityX << " " << velocityY << " " << obstacle_scale << "\n";*/

                }
                //outFile << "\n";
            }
            outFile.close();
            // Level exported successfully
        }
    }

    //Animation______________________________
    ANIMATION::asteroid.Anim_Update(dt);
    //---------------------------------------
}

void LevelEditor_Draw() {
    // Clear the screen & Set to Default
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxTextureSet(nullptr, 0, 0);
    /******************************************************************************/
    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();

    f32 startX = -halfW + uiWidth;
    f32 startY = -halfH;

    AEMtx33 scale, trans, transform;

    // 1. Draw Grid Canvas
    for (int r = 0; r < VIEW_ROWS; ++r) {
        for (int c = 0; c < VIEW_COLS; ++c) {
            int mapCol = c + viewOffsetX;
            if (mapCol >= mapTiles[0].size()) continue;

            int tile = mapTiles[r][mapCol].type;

            f32 pX = startX + (c * TILE_SIZE) + (TILE_SIZE / 2.0f);
            f32 pY = startY + (r * TILE_SIZE) + (TILE_SIZE / 2.0f);
            AEMtx33Trans(&trans, pX, pY);

            if (tile == 0) {
                AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                AEGfxTextureSet(NULL, 0, 0);
                AEMtx33Scale(&scale, TILE_SIZE - 2.0f, TILE_SIZE - 2.0f);
                AEMtx33Concat(&transform, &trans, &scale);
                AEGfxSetTransform(transform.m);
                AEGfxMeshDraw(meshGrid, AE_GFX_MDM_TRIANGLES);
            }
            else {
                // Set render/texture state BEFORE transform to avoid state flush ordering issues.
                // ColorToMultiply MUST be set immediately before MeshDraw (AlphaEngine consumes it per-draw).
                if (tile == Asteroid && ASSETS::backgroundAssets) {
                    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
                    AEGfxTextureSet(ASSETS::backgroundAssets, UV_ASTEROID_X, UV_ASTEROID_Y);
                }
                else if (tile == Spike && ASSETS::backgroundAssets) {
                    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
                    AEGfxTextureSet(ASSETS::backgroundAssets, UV_SPIKE_X, UV_SPIKE_Y);
                }
                else {
                    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                    AEGfxTextureSet(NULL, 0, 0);
                }
                AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
                AEMtx33Concat(&transform, &trans, &scale);
                AEGfxSetTransform(transform.m);
                AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
                AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
                AEGfxMeshDraw(meshIcon, AE_GFX_MDM_TRIANGLES);
            }
        }
    }

    // 2. Draw Left UI Panel Background
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0, 0);
    AEMtx33Scale(&scale, uiWidth, halfH * 2.0f);
    AEMtx33Trans(&trans, -halfW + (uiWidth / 2.0f), 0.0f);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(meshUIBg, AE_GFX_MDM_TRIANGLES);

    // 3. Draw UI Tool Slots
    f32 slotCenterX = -halfW + (uiWidth / 2.0f);
    f32 slot1Y = halfH - 120.0f;
    f32 slot2Y = halfH - 260.0f;
    f32 slotSize = 100.0f;
    f32 iconSize = 70.0f;

    // DrawToolBox uses Anim_Draw which resets ColorToMultiply/Add before texturing,
    // preventing color-state bleed from preceding COLOR-mode mesh draws.
    auto DrawToolBox = [&](f32 sY, ObstacleType type, ANIMATION::AnimatedSprite& anim) {
        if (currentTool == type) {
            AEMtx33Scale(&scale, slotSize + 8.0f, slotSize + 8.0f);
            AEMtx33Trans(&trans, slotCenterX, sY);
            AEMtx33Concat(&transform, &trans, &scale);
            AEGfxSetTransform(transform.m);
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0, 0);
            AEGfxMeshDraw(meshSlotBorder, AE_GFX_MDM_TRIANGLES);
        }

        AEMtx33Scale(&scale, slotSize, slotSize);
        AEMtx33Trans(&trans, slotCenterX, sY);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxTextureSet(NULL, 0, 0);
        AEGfxMeshDraw(meshSlotBg, AE_GFX_MDM_TRIANGLES);

        // Anim_Draw sets TEXTURE mode + resets color state.
        // ColorToMultiply MUST be re-set immediately before MeshDraw (AlphaEngine consumes it per-draw).
        anim.Anim_Draw(ASSETS::backgroundAssets);
        AEMtx33Scale(&scale, iconSize, iconSize);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEGfxMeshDraw(meshIcon, AE_GFX_MDM_TRIANGLES);
        };

    DrawToolBox(slot1Y, Asteroid, ANIMATION::asteroid);
    DrawToolBox(slot2Y, Spike,    ANIMATION::spike);

    // 4. Draw drag preview attached to mouse cursor
    if (isDragging && mouseX < -halfW + uiWidth) {
        if (currentTool == Asteroid && ASSETS::backgroundAssets) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxTextureSet(ASSETS::backgroundAssets, UV_ASTEROID_X, UV_ASTEROID_Y);
        }
        else if (currentTool == Spike && ASSETS::backgroundAssets) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxTextureSet(ASSETS::backgroundAssets, UV_SPIKE_X, UV_SPIKE_Y);
        }
        else {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0, 0);
        }
        AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
        AEMtx33Trans(&trans, mouseX, mouseY);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
        AEGfxMeshDraw(meshIcon, AE_GFX_MDM_TRIANGLES);
    }
    
    // 5. Draw UI Text (representing velocity and scale values)
    //f32 textX = slotCenterX;  // small padding from left panel edge
    f32 textY = 0.0;    // start center
    f32 lineHeight = 0.2f;       // vertical spacing between lines

    char buffer[128];
    // ===================================================================
    // Example: showing player velocity use switch to change text
    char const* obstacle_velocity_text{ "Normal" };
    switch (obstacle_speed.first) {
    case ObstacleSpeed::Slow:
        obstacle_velocity_text = "Slow";
        break;
    case ObstacleSpeed::Normal:
        obstacle_velocity_text = "Normal";
        break;
    case ObstacleSpeed::Fast:
        obstacle_velocity_text = "Fast";
        break;
    }
    sprintf_s(buffer, "Horizontal: %s", obstacle_velocity_text);
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.5f, 0.9f, 0.9f, 0.2f, 1.0f);
    textY -= lineHeight;  // move down for next line
    // =====

    switch (obstacle_speed.second) {
    case ObstacleSpeed::Slow:
        obstacle_velocity_text = "Slow";
        break;
    case ObstacleSpeed::Normal:
        obstacle_velocity_text = "Normal";
        break;
    case ObstacleSpeed::Fast:
        obstacle_velocity_text = "Fast";
        break;
    }
    sprintf_s(buffer, "Vertical: %s", obstacle_velocity_text);
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.5f, 0.9f, 0.9f, 0.2f, 1.0f);
    textY -= lineHeight;
    // ===================================================================
    // Example: showing tile scale text
    char const* obstacle_size_text{ "Normal" };
    switch (obstacle_size) {
        case ObstacleScale::Tiny:
            obstacle_size_text = "Tiny";
            break;
        case ObstacleScale::Small:
            obstacle_size_text = "Small";
            break;
        case ObstacleScale::Normal:
            obstacle_size_text = "Normal";
            break;
        case ObstacleScale::Large:
            obstacle_size_text = "Large";
            break;
        case ObstacleScale::Giant:
            obstacle_size_text = "Giant";
            break;
        }
    // ========================================================================
    sprintf_s(buffer, "Scale Size: %s", obstacle_size_text);
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.5f, 0.9f, 0.9f, 0.2f, 1.0f);
    textY -= lineHeight;

    // Example: showing current tool
    sprintf_s(buffer, "Tool: %s", currentTool == Asteroid ? "Asteroid" : "Spike");
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.5f, 0.9f, 0.9f, 0.2f, 1.0f);
    //------------------------------------------------------------------------------

    
}

void LevelEditor_Free() {
    mapTiles.clear();
    std::vector<std::vector<LevelTile>>().swap(mapTiles);

    if (meshWhite)      { AEGfxMeshFree(meshWhite);      meshWhite      = nullptr; }
    if (meshUIBg)       { AEGfxMeshFree(meshUIBg);       meshUIBg       = nullptr; }
    if (meshSlotBg)     { AEGfxMeshFree(meshSlotBg);     meshSlotBg     = nullptr; }
    if (meshSlotBorder) { AEGfxMeshFree(meshSlotBorder); meshSlotBorder = nullptr; }
    if (meshGrid)       { AEGfxMeshFree(meshGrid);       meshGrid       = nullptr; }
    if (meshIcon)       { AEGfxMeshFree(meshIcon);       meshIcon       = nullptr; }
}

void LevelEditor_Unload() {
    ASSETS::Unload_Font();
    ASSETS::Unload_Images();
}