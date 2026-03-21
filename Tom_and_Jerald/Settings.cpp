#include "pch.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Settings.hpp"
#include "Audio.hpp"

namespace {
    // all sliders stored in one vector
    std::vector<Slider> sliders;
    s8 font_id;
}

// returns current volume value between 0.0f (silent) and 1.0f (full)
float Slider::GetValue() const { return mValue; }
// returns the slider label
const char* Slider::GetLabel() const { return mLabel.c_str(); }
// returns the audio group so Settings can apply volume changes
AEAudioGroup AudioClip::GetGroup() const { return mGroup; }

// reads SliderConfig.txt and initializes all sliders
// same >> pattern as AudioLoadConfig
bool SliderLoadConfig(const char* filename)
{
    std::ifstream ifs(filename);
    if (!ifs)
    {
        std::cerr << "Cannot open " << filename << "\n";
        return false;
    }

    std::string label;
    float x, y, width, height;

    // each line creates a new slider and adds it to the vector
    while (ifs >> label >> x >> y >> width >> height)
    {
        Slider s;
        s.Init(label.c_str(), x, y, width, height);
        sliders.push_back(s); // automatically grows the vector
    }

    ifs.close();
    return true;
}

// initializes slider with label and position passed in from config reader
void Slider::Init(const char* label, float x, float y, float width, float height)
{
    mLabel = label;
    mX = x;
    mY = y;
    mWidth = width;
    mHeight = height;
    mValue = 1.f; // default to full value
    mIsDragging = false;
}

void Slider::Update()
{
    // get mouse position in screen space
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);

    // convert mouse screen position to world position
    float worldMouseX = (float)mouseX - (AEGfxGetWinMaxX());
    float worldMouseY = -((float)mouseY - (AEGfxGetWinMaxY()));

    // calculate left and right edge of the track
    f32 handleX = (mValue * 2.f - 1.f) * mWidth;

    // check if mouse is on the handle
    bool mouseOnHandle = (worldMouseX >= handleX - 10.f && worldMouseX <= handleX + 10.f &&
        worldMouseY >= mY - mHeight && worldMouseY <= mY + mHeight);

    // start dragging when left mouse button is pressed on handle
    if (AEInputCheckTriggered(AEVK_LBUTTON) && mouseOnHandle) {
        mIsDragging = true;
    }

    // stop dragging when left mouse button is released
    if (AEInputCheckReleased(AEVK_LBUTTON)) {
        mIsDragging = false;
    }

    // move handle with mouse while dragging, clamped within track bounds
    if (mIsDragging)
    {
        // clamp worldMouseX to track range
        float clampedX = worldMouseX;
        if (clampedX < -mWidth) clampedX = -mWidth;
        if (clampedX > mWidth) clampedX = mWidth;

        // convert position to 0.0-1.0 — matches draw formula
        mValue = (clampedX + mWidth) / (mWidth * 2.f);
    }
}

// creates track and handle meshes for drawing
void Slider::CreateMeshes()
{
    // track mesh (flat bar)
    AEGfxMeshStart();
    AEGfxTriAdd(-1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
    AEGfxTriAdd(1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
    trackMesh = AEGfxMeshEnd();

    // handle mesh (small rectangle)
    AEGfxMeshStart();
    AEGfxTriAdd(-1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
    AEGfxTriAdd(1.f, -1.f, 0xFFFFFFFF, 0.f, 0.f,
        1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f,
        -1.f, 1.f, 0xFFFFFFFF, 0.f, 0.f);
    handleMesh = AEGfxMeshEnd();
}

// frees track and handle meshes to prevent memory leaks
void Slider::FreeMeshes()
{
    AEGfxMeshFree(trackMesh);
    AEGfxMeshFree(handleMesh);
    trackMesh = nullptr;
    handleMesh = nullptr;
}

void Slider::Draw(s8 font_id)
{
    AEMtx33 transform, scale, trans;

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetTransparency(1.f);

    // draw track bar
    AEMtx33Scale(&scale, mWidth, 2.f);
    AEMtx33Trans(&trans, 0.f, mY);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(trackMesh, AE_GFX_MDM_TRIANGLES);

    // draw handle
    f32 handleX = (mValue * 2.f - 1.f) * mWidth;
    AEMtx33Scale(&scale, 8.f, 15.f);
    AEMtx33Trans(&trans, handleX, mY);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);
    AEGfxMeshDraw(handleMesh, AE_GFX_MDM_TRIANGLES);

    // draw label and percentage 
    f32 labelY = mY / AEGfxGetWinMaxY();
    AEGfxPrint(font_id, mLabel.c_str(), -0.8f, labelY, 0.5f, 1.f, 1.f, 1.f, 1.f);
    char buffer[32];
    sprintf_s(buffer, 32, "%.0f%%", mValue * 100.f);
    AEGfxPrint(font_id, buffer, 0.7f, labelY, 0.5f, 1.f, 1.f, 1.f, 1.f);
}

// resets slider value and handle back to default
void Slider::Reset()
{
    mValue = 1.f;
    mIsDragging = false;
}

void Setting_Load() {
    // load font for settings screen
    font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void Settings_Initialize() {
    sliders.clear();
    SliderLoadConfig("Assets/SliderConfig.txt");

    // create meshes after sliders are initialized
    for (Slider& s : sliders) {
        s.CreateMeshes();
    }
}

void Settings_Update() {
    if (IsMenuKeyTriggered()) {
        clickAudio.Play();
    }

    for (Slider& s : sliders) {
        s.Update();
    }

    // apply slider values to audio groups by index
    if (sliders.size() > 0)
        AEAudioSetGroupVolume(backgroundAudio.GetGroup(), sliders[0].GetValue()); // bgm

    if (sliders.size() > 1)
    {
        AEAudioSetGroupVolume(clickAudio.GetGroup(), sliders[1].GetValue()); // sfx
        AEAudioSetGroupVolume(ratsqueakAudio.GetGroup(), sliders[1].GetValue()); // sfx
    }

    // go back to main menu on ESC
    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        next = GAME_STATE_MENU;
    }
}

// draws all sliders
void Settings_Draw() {
    for (Slider& s : sliders) {
        s.Draw(font_id);
    }
}

// frees slider memory
void Settings_Free() {
    for (Slider& s : sliders) {
        s.Reset();
        s.FreeMeshes();
    }
    sliders.clear();
}

void Settings_Unload() {
    AEGfxDestroyFont(font_id);
}