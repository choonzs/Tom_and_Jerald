#include "pch.hpp"
#include "LevelEditor.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include <vector>
#include <iostream>
#include <fstream>

namespace {
    // Dynamic Editor Configuration
    int VIEW_COLS = 0;              // Will be calculated dynamically based on screen width
    const int VIEW_ROWS = 36;       // The fixed vertical height of your level logic
    const int MAX_COLS = 200;       // Total map length

    f32 TILE_SIZE = 0.0f;           // Will stretch to fit screen vertically
    f32 uiWidth = 240.0f;           // The width of our new polished left toolbar

    std::vector<std::vector<int>> mapData;
    int viewOffsetX = 0;

    // Tool Selection State
    bool isDragging = false;
    int currentTool = TILE_SQUARE;  // Default selected tool
    f32 mouseX, mouseY;

    // Textures
    AEGfxTexture* texSquare = nullptr;
    AEGfxTexture* texSpike = nullptr;

    // Meshes for UI Styling
    AEGfxVertexList* meshWhite = nullptr;       // For Textures
    AEGfxVertexList* meshUIBg = nullptr;        // Dark grey toolbar background
    AEGfxVertexList* meshSlotBg = nullptr;      // Lighter grey for asset boxes
    AEGfxVertexList* meshSlotBorder = nullptr;  // Orange highlight for selected tool
    AEGfxVertexList* meshGrid = nullptr;        // Faint grid lines

    // Helper function to easily generate colored square meshes
    AEGfxVertexList* CreateSquareMesh(u32 color) {
        AEGfxMeshStart();
        AEGfxTriAdd(
            -0.5f, -0.5f, color, 0.0f, 1.0f,
            0.5f, -0.5f, color, 1.0f, 1.0f,
            -0.5f, 0.5f, color, 0.0f, 0.0f);
        AEGfxTriAdd(
            0.5f, -0.5f, color, 1.0f, 1.0f,
            0.5f, 0.5f, color, 1.0f, 0.0f,
            -0.5f, 0.5f, color, 0.0f, 0.0f);
        return AEGfxMeshEnd();
    }
}

void LevelEditor_Load() {
    texSquare = AEGfxTextureLoad("Assets/Square.png");
    texSpike = AEGfxTextureLoad("Assets/Spike.png");

    // Initialize styling meshes (Colors are ARGB hex format)
    meshWhite = CreateSquareMesh(0xFFFFFFFF); // Solid White
    meshUIBg = CreateSquareMesh(0xFF2A2D34); // Slate Grey
    meshSlotBg = CreateSquareMesh(0xFF3E434C); // Lighter Grey
    meshSlotBorder = CreateSquareMesh(0xFFF9A03F); // Vibrant Orange
    meshGrid = CreateSquareMesh(0x1AFFFFFF); // 10% Opacity White
}

void LevelEditor_Initialize() {
    mapData.assign(VIEW_ROWS, std::vector<int>(MAX_COLS, TILE_EMPTY));
    viewOffsetX = 0;
    isDragging = false;
    currentTool = TILE_SQUARE;

    AEGfxSetCamPosition(0.0f, 0.0f);
    AEGfxSetBackgroundColor(0.15f, 0.15f, 0.15f); // Darker background for canvas
}

void LevelEditor_Update() {
    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        next = GAME_STATE_MENU;
        return;
    }

    // Camera Scrolling
    if (AEInputCheckTriggered(AEVK_RIGHT) && viewOffsetX < MAX_COLS - VIEW_COLS) viewOffsetX++;
    if (AEInputCheckTriggered(AEVK_LEFT) && viewOffsetX > 0) viewOffsetX--;

    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();

    // Dynamically scale tile size so that the 36 rows perfectly fit the window height
    TILE_SIZE = (halfH * 2.0f) / VIEW_ROWS;
    VIEW_COLS = (int)(((halfW * 2.0f) - uiWidth) / TILE_SIZE) + 1;

    mouseX = (f32)mx - halfW;
    mouseY = -(f32)my + halfH;

    // --- LEFT MOUSE LOGIC: UI Selection & Painting ---
    if (AEInputCheckCurr(AEVK_LBUTTON)) {
        if (mouseX < -halfW + uiWidth) {
            // Clicked inside UI Sidebar (Only register initial clicks to prevent spam)
            if (AEInputCheckTriggered(AEVK_LBUTTON)) {
                f32 slot1Y = halfH - 120.0f;
                f32 slot2Y = halfH - 260.0f;

                // Check collision with the UI tool boxes
                if (mouseY < slot1Y + 50.0f && mouseY > slot1Y - 50.0f) { currentTool = TILE_SQUARE; isDragging = true; }
                if (mouseY < slot2Y + 50.0f && mouseY > slot2Y - 50.0f) { currentTool = TILE_SPIKE; isDragging = true; }
            }
        }
        else {
            // Clicked inside Grid Canvas - Paint the current tool!
            int gridX = (int)((mouseX - (-halfW + uiWidth)) / TILE_SIZE) + viewOffsetX;
            int gridY = (int)((mouseY + halfH) / TILE_SIZE);
            if (gridX >= 0 && gridX < MAX_COLS && gridY >= 0 && gridY < VIEW_ROWS) {
                mapData[gridY][gridX] = currentTool;
            }
        }
    }

    // --- RIGHT MOUSE LOGIC: Erasing ---
    if (AEInputCheckCurr(AEVK_RBUTTON)) {
        if (mouseX >= -halfW + uiWidth) {
            int gridX = (int)((mouseX - (-halfW + uiWidth)) / TILE_SIZE) + viewOffsetX;
            int gridY = (int)((mouseY + halfH) / TILE_SIZE);
            if (gridX >= 0 && gridX < MAX_COLS && gridY >= 0 && gridY < VIEW_ROWS) {
                mapData[gridY][gridX] = TILE_EMPTY; // Erase tile
            }
        }
    }

    if (AEInputCheckReleased(AEVK_LBUTTON)) {
        isDragging = false; // Stop drawing tool attached to cursor
    }

    // Exporting
    if (AEInputCheckTriggered(AEVK_E)) {
        std::ofstream outFile("ExportedLevel.txt");
        if (outFile.is_open()) {
            outFile << MAX_COLS << " " << VIEW_ROWS << "\n";
            for (int r = 0; r < VIEW_ROWS; ++r) {
                for (int c = 0; c < MAX_COLS; ++c) {
                    outFile << mapData[r][c] << " ";
                }
                outFile << "\n";
            }
            outFile.close();
            std::cout << "Level Exported!\n";
        }
    }
}

void LevelEditor_Draw() {
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();

    f32 startX = -halfW + uiWidth; // Grid starts AFTER the UI panel
    f32 startY = -halfH;

    AEMtx33 scale, trans, transform;

    // 1. Draw Grid Canvas
    for (int r = 0; r < VIEW_ROWS; ++r) {
        for (int c = 0; c < VIEW_COLS; ++c) {
            int mapCol = c + viewOffsetX;
            if (mapCol >= MAX_COLS) continue; // Don't draw past map bounds

            int tile = mapData[r][mapCol];

            f32 pX = startX + (c * TILE_SIZE) + (TILE_SIZE / 2.0f);
            f32 pY = startY + (r * TILE_SIZE) + (TILE_SIZE / 2.0f);
            AEMtx33Trans(&trans, pX, pY);

            if (tile == TILE_EMPTY) {
                // Draw faint grid wireframe
                AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                AEGfxTextureSet(NULL, 0, 0);
                AEMtx33Scale(&scale, TILE_SIZE - 2.0f, TILE_SIZE - 2.0f); // -2 to create grid lines
                AEMtx33Concat(&transform, &trans, &scale);
                AEGfxSetTransform(transform.m);
                AEGfxMeshDraw(meshGrid, AE_GFX_MDM_TRIANGLES);
            }
            else {
                // Draw placed physical tiles
                AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
                AEMtx33Concat(&transform, &trans, &scale);
                AEGfxSetTransform(transform.m);

                if (tile == TILE_SQUARE && texSquare) {
                    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
                    AEGfxTextureSet(texSquare, 0, 0);
                }
                else if (tile == TILE_SPIKE && texSpike) {
                    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
                    AEGfxTextureSet(texSpike, 0, 0);
                }
                else {
                    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
                    AEGfxTextureSet(NULL, 0, 0); // Fallback solid white
                }
                AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
            }
        }
    }

    // 2. Draw Left UI Panel Background
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxTextureSet(NULL, 0, 0);
    AEMtx33Scale(&scale, uiWidth, halfH * 2.0f);
    AEMtx33Trans(&trans, -halfW + (uiWidth / 2.0f), 0.0f); // Center vertically
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(meshUIBg, AE_GFX_MDM_TRIANGLES);

    // 3. Draw UI Tool Slots
    f32 slotCenterX = -halfW + (uiWidth / 2.0f);
    f32 slot1Y = halfH - 120.0f;
    f32 slot2Y = halfH - 260.0f;
    f32 slotSize = 100.0f;
    f32 iconSize = 70.0f;

    // A handy lambda function to draw tool boxes cleanly
    auto DrawToolBox = [&](f32 sY, int tileType, AEGfxTexture* tex) {
        // Draw Orange Border if selected
        if (currentTool == tileType) {
            AEMtx33Scale(&scale, slotSize + 8.0f, slotSize + 8.0f);
            AEMtx33Trans(&trans, slotCenterX, sY);
            AEMtx33Concat(&transform, &trans, &scale);
            AEGfxSetTransform(transform.m);
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0, 0);
            AEGfxMeshDraw(meshSlotBorder, AE_GFX_MDM_TRIANGLES);
        }

        // Draw Slot Background
        AEMtx33Scale(&scale, slotSize, slotSize);
        AEMtx33Trans(&trans, slotCenterX, sY);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxTextureSet(NULL, 0, 0);
        AEGfxMeshDraw(meshSlotBg, AE_GFX_MDM_TRIANGLES);

        // Draw Icon Inside
        AEMtx33Scale(&scale, iconSize, iconSize);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        if (tex) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxTextureSet(tex, 0, 0);
        }
        else {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0, 0);
        }
        AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
        };

    DrawToolBox(slot1Y, TILE_SQUARE, texSquare);
    DrawToolBox(slot2Y, TILE_SPIKE, texSpike);

    // 4. Draw drag preview attached to mouse cursor
    if (isDragging && mouseX < -halfW + uiWidth) {
        AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
        AEMtx33Trans(&trans, mouseX, mouseY);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);

        if (currentTool == TILE_SQUARE && texSquare) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxTextureSet(texSquare, 0, 0);
        }
        else if (currentTool == TILE_SPIKE && texSpike) {
            AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
            AEGfxTextureSet(texSpike, 0, 0);
        }
        else {
            AEGfxSetRenderMode(AE_GFX_RM_COLOR);
            AEGfxTextureSet(NULL, 0, 0);
        }
        AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
    }
}

void LevelEditor_Free() {
    mapData.clear();
}

void LevelEditor_Unload() {
    if (texSquare) AEGfxTextureUnload(texSquare);
    if (texSpike) AEGfxTextureUnload(texSpike);

    // Clean up all styling meshes
    if (meshWhite) AEGfxMeshFree(meshWhite);
    if (meshUIBg) AEGfxMeshFree(meshUIBg);
    if (meshSlotBg) AEGfxMeshFree(meshSlotBg);
    if (meshSlotBorder) AEGfxMeshFree(meshSlotBorder);
    if (meshGrid) AEGfxMeshFree(meshGrid);
}