#include "pch.hpp"
#include "LevelEditor.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Obstacle.hpp"
#include "ImgFontInit.hpp"
#include "LevelTile.hpp"
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
    f32 velocityX{};
    f32 velocityY{};

    f32 obstacle_scale{1.0f}; //by default

    // Textures & Meshes
    AEGfxTexture* texSquare = nullptr;
    AEGfxTexture* texSpike = nullptr;

    AEGfxVertexList* meshWhite = nullptr;
    AEGfxVertexList* meshUIBg = nullptr;
    AEGfxVertexList* meshSlotBg = nullptr;
    AEGfxVertexList* meshSlotBorder = nullptr;
    AEGfxVertexList* meshGrid = nullptr;

    AEGfxVertexList* CreateSquareMesh(u32 color) {
        AEGfxMeshStart();
        AEGfxTriAdd(-0.5f, -0.5f, color, 0.0f, 1.0f, 0.5f, -0.5f, color, 1.0f, 1.0f, -0.5f, 0.5f, color, 0.0f, 0.0f);
        AEGfxTriAdd(0.5f, -0.5f, color, 1.0f, 1.0f, 0.5f, 0.5f, color, 1.0f, 0.0f, -0.5f, 0.5f, color, 0.0f, 0.0f);
        return AEGfxMeshEnd();
    }
}

void LevelEditor_Load() {
    texSquare = AEGfxTextureLoad("Assets/Square.png");
    texSpike = AEGfxTextureLoad("Assets/Spike.png");
    ASSETS::Init_Font();
}

void LevelEditor_Initialize() {
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
}

void LevelEditor_Update() {
    if (AEInputCheckTriggered(AEVK_ESCAPE)) { next = GAME_STATE_MENU;}

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
	if (AEInputCheckCurr(AEVK_D)) velocityX += 10.0f;
	if (AEInputCheckCurr(AEVK_A)) velocityX -= 10.0f;
	if (AEInputCheckCurr(AEVK_W)) velocityY += 10.0f;
	if (AEInputCheckCurr(AEVK_S)) velocityY -= 10.0f;
	// Scale adjustments with Z and X keys
	if (AEInputCheckCurr(AEVK_Z)) obstacle_scale += .1f;
	if (AEInputCheckCurr(AEVK_X)) obstacle_scale -= .1f;


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
                    AEVec2({ (TILE_SIZE / 2.0f) * obstacle_scale, (TILE_SIZE / 2.0f) * obstacle_scale }),
                    AEVec2({ velocityX, velocityY }),
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
                //mapData[gridY][gridX] = 0; // 0 = Empty Tile
                mapTiles[gridY].erase(mapTiles[gridY].begin() + gridX);
                
            }
        }
    }

    if (AEInputCheckReleased(AEVK_LBUTTON)) isDragging = false;

    // --- EXPORTING ---
    if (AEInputCheckTriggered(AEVK_E)) {
		//TODO Get Max window position and set it as finish line for the level end
        int fileIndex = 1;
		std::string filename;

        // Shorten for easier readability
		namespace fs = std::filesystem;

		// Create "MapLevel" directory if it doesn't exist
        if (fs::create_directory("MapLevel")) {
            //std::cout << "Directory 'MapLevel' created successfully.\n";
		}

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
                    //outFile << mapData[r][c] << " ";
                    //
                    outFile << mapTiles[r][c].type << " " << mapTiles[r][c].pos.x << " " << mapTiles[r][c].pos.y << " "
                            << mapTiles[r][c].half_size.x << " " << mapTiles[r][c].half_size.y << " "
						<< velocityX << " " << velocityY << " " << obstacle_scale << "\n";

                }
                outFile << "\n";
            }
            outFile.close();
            std::cout << "Level Exported dynamically with " << currentMaxCols << " columns!\n";
        }
    }
}

void LevelEditor_Draw() {
    // Clear the screen & Set to Default
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 0.0f);
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
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
                AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
                AEMtx33Concat(&transform, &trans, &scale);
                AEGfxSetTransform(transform.m);

                if (tile == 1 && texSquare) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(texSquare, 0, 0); }
                else if (tile == 2 && texSpike) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(texSpike, 0, 0); }
                else { AEGfxSetRenderMode(AE_GFX_RM_COLOR); AEGfxTextureSet(NULL, 0, 0); }
                AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
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

    auto DrawToolBox = [&](f32 sY, int tileType, AEGfxTexture* tex) {
        if (currentTool == tileType) {
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

        AEMtx33Scale(&scale, iconSize, iconSize);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        if (tex) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(tex, 0, 0); }
        else { AEGfxSetRenderMode(AE_GFX_RM_COLOR); AEGfxTextureSet(NULL, 0, 0); }
        AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
        };

    DrawToolBox(slot1Y, Asteroid, texSquare);
    DrawToolBox(slot2Y, Spike, texSpike);

    // 4. Draw drag preview attached to mouse cursor
    if (isDragging && mouseX < -halfW + uiWidth) {
        AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
        AEMtx33Trans(&trans, mouseX, mouseY);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);

        if (currentTool == Asteroid && texSquare) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(texSquare, 0, 0); }
        else if (currentTool == Spike && texSpike) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(texSpike, 0, 0); }
        else { AEGfxSetRenderMode(AE_GFX_RM_COLOR); AEGfxTextureSet(NULL, 0, 0); }
        AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
    }
    
    // 5. Draw UI Text (representing velocity and scale values)
    f32 textX = slotCenterX;  // small padding from left panel edge
    f32 textY = 0.0;    // start center
    f32 lineHeight = 0.2f;       // vertical spacing between lines

    char buffer[128];

    // Example: showing player velocity
    sprintf_s(buffer, "Velocity X: %.2f", velocityX);
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.4f, 0.9f, 0.9f, 0.2f, 1.0f);
    textY -= lineHeight;  // move down for next line

    sprintf_s(buffer, "Velocity Y: %.2f", velocityY);
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.4f, 0.9f, 0.9f, 0.2f, 1.0f);
    textY -= lineHeight;

    // Example: showing tile scale
    sprintf_s(buffer, "Scale Size: %.2f", static_cast<f32>(obstacle_scale * 100));
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.4f, 0.9f, 0.9f, 0.2f, 1.0f);
    textY -= lineHeight;

    // Example: showing current tool
    sprintf_s(buffer, "Current Tool: %s", currentTool == Asteroid ? "Asteroid" : "Spike");
    AEGfxPrint(ASSETS::Font(), buffer, -0.95f, textY, 0.4f, 0.9f, 0.9f, 0.2f, 1.0f);
    //------------------------------------------------------------------------------

    
}

void LevelEditor_Free() {
    mapTiles.clear();
    std::vector<std::vector<LevelTile>>().swap(mapTiles);

    if (meshWhite) AEGfxMeshFree(meshWhite);
    if (meshUIBg) AEGfxMeshFree(meshUIBg);
    if (meshSlotBg) AEGfxMeshFree(meshSlotBg);
    if (meshSlotBorder) AEGfxMeshFree(meshSlotBorder);
    if (meshGrid) AEGfxMeshFree(meshGrid);
}

void LevelEditor_Unload() {
    if (texSquare) AEGfxTextureUnload(texSquare);
    if (texSpike) AEGfxTextureUnload(texSpike);
    ASSETS::Unload_Font();
}