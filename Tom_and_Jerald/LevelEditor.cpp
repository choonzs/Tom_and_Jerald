#include "pch.hpp"
#include "LevelEditor.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include <vector>
#include <iostream>
#include <fstream>

namespace {
    int VIEW_COLS = 0;
    const int VIEW_ROWS = 36;
    f32 TILE_SIZE = 0.0f;
    f32 uiWidth = 240.0f;

    std::vector<std::vector<int>> mapData;
    int viewOffsetX = 0;
    f32 scrollTimer = 0.0f; // Controls smooth scrolling speed

    // Tool Selection State
    bool isDragging = false;
    int currentTool = 1;  // 1 = TILE_SQUARE
    f32 mouseX, mouseY;

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
}

void LevelEditor_Initialize() {
    // Start with 100 columns. It will grow infinitely as you scroll right!
    mapData.assign(VIEW_ROWS, std::vector<int>(100, 0));
    viewOffsetX = 0;
    isDragging = false;
    currentTool = 1;

    AEGfxSetCamPosition(0.0f, 0.0f);
    AEGfxSetBackgroundColor(0.15f, 0.15f, 0.15f);

    meshWhite = CreateSquareMesh(0xFFFFFFFF);
    meshUIBg = CreateSquareMesh(0xFF2A2D34);
    meshSlotBg = CreateSquareMesh(0xFF3E434C);
    meshSlotBorder = CreateSquareMesh(0xFFF9A03F);
    meshGrid = CreateSquareMesh(0x1AFFFFFF);
}

void LevelEditor_Update() {
    if (AEInputCheckTriggered(AEVK_ESCAPE)) { next = GAME_STATE_MENU; return; }

    f32 dt = (f32)AEFrameRateControllerGetFrameTime();
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();

    TILE_SIZE = (halfH * 2.0f) / VIEW_ROWS;
    VIEW_COLS = (int)(((halfW * 2.0f) - uiWidth) / TILE_SIZE) + 1;

    mouseX = (f32)mx - halfW;
    mouseY = -(f32)my + halfH;

    // --- SMOOTH SCROLLING & INFINITE DYNAMIC GENERATION ---
    if (AEInputCheckCurr(AEVK_RIGHT)) {
        scrollTimer += dt;
        if (scrollTimer > 0.05f) { // Scroll speed limit
            viewOffsetX++;
            scrollTimer = 0.0f;

            // DYNAMIC GENERATION: Append a new empty column if we reach the map's boundary
            if (viewOffsetX + VIEW_COLS >= mapData[0].size()) {
                for (int r = 0; r < VIEW_ROWS; ++r) {
                    mapData[r].push_back(0); // 0 = Empty tile
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
                if (mouseY < slot1Y + 50.0f && mouseY > slot1Y - 50.0f) { currentTool = 1; isDragging = true; } // SQUARE
                if (mouseY < slot2Y + 50.0f && mouseY > slot2Y - 50.0f) { currentTool = 2; isDragging = true; } // SPIKE
            }
        }
        else {
            int gridX = (int)((mouseX - (-halfW + uiWidth)) / TILE_SIZE) + viewOffsetX;
            int gridY = (int)((mouseY + halfH) / TILE_SIZE);
            if (gridX >= 0 && gridX < mapData[0].size() && gridY >= 0 && gridY < VIEW_ROWS) {
                mapData[gridY][gridX] = currentTool;
            }
        }
    }

    // --- RIGHT MOUSE LOGIC: Erasing ---
    if (AEInputCheckCurr(AEVK_RBUTTON)) {
        if (mouseX >= -halfW + uiWidth) {
            int gridX = (int)((mouseX - (-halfW + uiWidth)) / TILE_SIZE) + viewOffsetX;
            int gridY = (int)((mouseY + halfH) / TILE_SIZE);
            if (gridX >= 0 && gridX < mapData[0].size() && gridY >= 0 && gridY < VIEW_ROWS) {
                mapData[gridY][gridX] = 0; // 0 = Empty Tile
            }
        }
    }

    if (AEInputCheckReleased(AEVK_LBUTTON)) isDragging = false;

    // --- EXPORTING ---
    if (AEInputCheckTriggered(AEVK_E)) {
        std::ofstream outFile("Level1.txt"); // Saves exactly what Level 1 will read
        if (outFile.is_open()) {
            int currentMaxCols = mapData[0].size();
            outFile << currentMaxCols << " " << VIEW_ROWS << "\n";
            for (int r = 0; r < VIEW_ROWS; ++r) {
                for (int c = 0; c < currentMaxCols; ++c) {
                    outFile << mapData[r][c] << " ";
                }
                outFile << "\n";
            }
            outFile.close();
            std::cout << "Level Exported dynamically with " << currentMaxCols << " columns!\n";
        }
    }
}

void LevelEditor_Draw() {
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    f32 halfW = AEGfxGetWinMaxX();
    f32 halfH = AEGfxGetWinMaxY();

    f32 startX = -halfW + uiWidth;
    f32 startY = -halfH;

    AEMtx33 scale, trans, transform;

    // 1. Draw Grid Canvas
    for (int r = 0; r < VIEW_ROWS; ++r) {
        for (int c = 0; c < VIEW_COLS; ++c) {
            int mapCol = c + viewOffsetX;
            if (mapCol >= mapData[0].size()) continue;

            int tile = mapData[r][mapCol];

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

    DrawToolBox(slot1Y, 1, texSquare);
    DrawToolBox(slot2Y, 2, texSpike);

    // 4. Draw drag preview attached to mouse cursor
    if (isDragging && mouseX < -halfW + uiWidth) {
        AEMtx33Scale(&scale, TILE_SIZE, TILE_SIZE);
        AEMtx33Trans(&trans, mouseX, mouseY);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);

        if (currentTool == 1 && texSquare) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(texSquare, 0, 0); }
        else if (currentTool == 2 && texSpike) { AEGfxSetRenderMode(AE_GFX_RM_TEXTURE); AEGfxTextureSet(texSpike, 0, 0); }
        else { AEGfxSetRenderMode(AE_GFX_RM_COLOR); AEGfxTextureSet(NULL, 0, 0); }
        AEGfxMeshDraw(meshWhite, AE_GFX_MDM_TRIANGLES);
    }
}

void LevelEditor_Free() {
    mapData.clear();

    if (meshWhite) AEGfxMeshFree(meshWhite);
    if (meshUIBg) AEGfxMeshFree(meshUIBg);
    if (meshSlotBg) AEGfxMeshFree(meshSlotBg);
    if (meshSlotBorder) AEGfxMeshFree(meshSlotBorder);
    if (meshGrid) AEGfxMeshFree(meshGrid);
}

void LevelEditor_Unload() {
    if (texSquare) AEGfxTextureUnload(texSquare);
    if (texSpike) AEGfxTextureUnload(texSpike);
}