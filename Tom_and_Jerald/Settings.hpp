/*************************************************************************
@file    Settings.hpp
@Author  Ong Jin Ting o.jinting@digipen.edu
@Co-authors  NIL
@brief
     Declares the Settings game-state lifecycle functions and the
     VolumeSlider helper class.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#pragma once
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include "pch.hpp"
#include "Audio.hpp"

class Slider {
	public:
        // initializes slider with label and position from config reader
        void Init(const char* label, float x, float y, float width, float height);
        void Update();
        void Draw(s8 font_id);
        void Reset();

        // creates the track and handle meshes for drawing
        void CreateMeshes();
        // frees the track and handle meshes
        void FreeMeshes();

        void SetValue(float v) { mValue = v; }

        float GetValue() const;        // returns value between 0.0f and 1.0f
        const char* GetLabel() const;  // returns the label

        private:
            std::string mLabel;
            float mX = 0.f;    // center x position of the slider track
            float mY = 0.f;    // center y position of the slider track
            float mWidth = 0.f;    // total width of the slider track
            float mHeight = 0.f;    // height of the slider track
            float mValue = 1.f;    // current value between 0.0f and 1.0f
            bool  mIsDragging = false;  // tracks if the handle is being dragged
            AEGfxVertexList* trackMesh = nullptr; // mesh for the slider track bar
            AEGfxVertexList* handleMesh = nullptr; // mesh for the draggable handle
};

// reads SliderConfig.txt and initializes all sliders
bool SliderLoadConfig(const char* filename);

void Setting_Load();
void Settings_Initialize();
void Settings_Update();
void Settings_Draw();
void Settings_Free();
void Settings_Unload();

#endif