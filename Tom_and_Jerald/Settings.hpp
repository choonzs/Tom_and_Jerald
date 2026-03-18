#pragma once
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "AEEngine.h"
#include "Audio.hpp"
#include <vector>

class Slider {
    private:

        const char* mLabel = nullptr;
        float mX = 0.f;
        float mY = 0.f;
        float mWidth = 0.f;
        float mHeight = 0.f;
        float mValue = 1.f;
        float mHandleX = 0.f;
        bool  mIsDragging = false;
//        f32 posY; // where the slider sits up/down on screen
//        f32 maxX; // how far left and right the slider stretches
//        f32 clickZone; // extra click area above and below the slider
//
//        std::vector<AEAudioGroup> audioGroups; // list of audio groups this slider controls
//
//        AEGfxVertexList* trackMesh; // mesh for the slider bar
//        AEGfxVertexList* handleMesh; // mesh for the draggable handle
//
//        // updates all bound audio groups to match current slider value
//        void ApplyVolume() {
//            for (AEAudioGroup& group : audioGroups) {
//                AEAudioSetGroupVolume(group, value);
//            }
//        }
//
	public:
//        f32 value; // current volume 0.0 (silent) to 1.0 (full)
//        const char* label; // display label shown to the left of the slider
//        f32 labelY; // normalized screen Y for label and percentage text
//
        Slider() = default;

        // position is passed in from the loop, not hardcoded per slider
        void Init(const char* label, float x, float y, float width, float height);
        void Update();
        void Draw();
        void Reset();

        float GetValue() const;
//
//        // constructs a slider with position, range, click area, and label
//        Slider(f32 value, f32 posY, f32 maxX, f32 clickZone, const char* label, f32 labelY)
//            : value(value), posY(posY), maxX(maxX), clickZone(clickZone), label(label), labelY(labelY), trackMesh(nullptr), handleMesh(nullptr) {
//        }
//
//        // binds an audio group to this slider — no limit on how many can be added
//        void AddAudioGroup(AEAudioGroup group) {
//            audioGroups.push_back(group);
//        }
//
//        // sync all bound audio groups to the current value on init
//        void ApplyDefaultVolume() {
//            ApplyVolume();
//        }
//
//        // Returns true if slider was interacted with this frame
//        bool UpdateFromMouse(f32 mouseX, f32 mouseY, bool justClicked) {
//            // check if mouse is close enough to the slider
//            bool inRange = (mouseY >= posY - clickZone && mouseY <= posY + clickZone);
//            if (!inRange) return false;
//
//            if (justClicked) PlayClick();
//
//            // turn mouse X position into a 0.0 to 1.0 value
//            f32 clampedX = AEClamp(mouseX, -maxX, maxX);
//            value = (clampedX + maxX) / (maxX * 2.0f);
//
//            // update volume on all audio groups
//            ApplyVolume();
//
//            return true;
//        }
//
        //void CreateMeshes() {
        //    // track mesh (flat bar)
        //    AEGfxMeshStart();
        //    AEGfxTriAdd(-1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
        //    AEGfxTriAdd(1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
        //    trackMesh = AEGfxMeshEnd();

        //    // handle mesh (small rectangle)
        //    AEGfxMeshStart();
        //    AEGfxTriAdd(-1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
        //    AEGfxTriAdd(1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f,
        //        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
        //    handleMesh = AEGfxMeshEnd();
        //}

        //void Draw(s8 font_id) {
        //    AEMtx33 transform, scale, trans;

        //    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        //    AEGfxSetTransparency(1.f);

        //    // draw track bar
        //    AEMtx33Scale(&scale, maxX, 2.f);
        //    AEMtx33Trans(&trans, 0.f, posY);
        //    AEMtx33Concat(&transform, &trans, &scale);
        //    AEGfxSetTransform(transform.m);
        //    AEGfxMeshDraw(trackMesh, AE_GFX_MDM_TRIANGLES);

        //    // draw handle — map value (0.0-1.0) back to screen space
        //    f32 handleX = (value * 2.f - 1.f) * maxX;
        //    AEMtx33Scale(&scale, 8.f, 15.f);
        //    AEMtx33Trans(&trans, handleX, posY);
        //    AEMtx33Concat(&transform, &trans, &scale);
        //    AEGfxSetTransform(transform.m);
        //    AEGfxMeshDraw(handleMesh, AE_GFX_MDM_TRIANGLES);

        //    // draw label and percentage
        //    AEGfxPrint(font_id, label, -0.8f, labelY, 0.5f, 1.f, 1.f, 1.f, 1.f);
        //    char buffer[32];
        //    sprintf_s(buffer, 32, "%.0f%%", value * 100.f);
        //    AEGfxPrint(font_id, buffer, 0.7f, labelY, 0.5f, 1.f, 1.f, 1.f, 1.f);
        //}
        //
        //// frees meshes and clears audio group bindings
        //void Free() {
        //    AEGfxMeshFree(trackMesh);
        //    AEGfxMeshFree(handleMesh);
        //    trackMesh = nullptr;
        //    handleMesh = nullptr;
        //    audioGroups.clear();
        //}
};

void Setting_Load();
void Settings_Initialize();
void Settings_Update();
void Settings_Draw();
void Settings_Free();
void Settings_Unload();

#endif