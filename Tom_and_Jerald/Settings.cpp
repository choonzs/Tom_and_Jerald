#include "pch.hpp"
#include "GameStateManager.hpp"
#include "GameStateList.hpp"
#include "Settings.hpp"
#include "Audio.hpp"

namespace {
    s8 font_id; // font used to draw labels and percentages
    std::vector<Slider> sliders; // all sliders on the settings screen
}

namespace SliderConfig {
    constexpr f32 track_width = 300.f; // width of the slider
    constexpr f32 clickZone = 25.f; // how far above/below the bar the mouse still registers
    constexpr f32 start_y = 100.f;  // screen height of the first slider
    constexpr f32 spacing_y = 100.f;  // gap between each slider
    constexpr f32 default_value = 1.0f; // starting volume for all sliders (1.0 = 100%)
}

// Helper functon -  build a slider and bind its audio groups 
static Slider MakeSlider(int index, const char* label, std::initializer_list<AEAudioGroup> groups)
{
    // place each slider below the previous one
    f32 posY = SliderConfig::start_y - index * SliderConfig::spacing_y;

    // convert screen position to normalized value for text drawing
    f32 labelY = posY / AEGfxGetWinMaxY();

    Slider s(SliderConfig::default_value, posY, SliderConfig::track_width, SliderConfig::clickZone, label, labelY);

    // connect each audio group to this slider
    for (AEAudioGroup g : groups) {
        s.AddAudioGroup(g);
    }

    return s;
}

void Setting_Load() {
    // load font for settings screen
    font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
}

void Settings_Initialize() {
    sliders.clear();

    // To add a new slider: just add one more MakeSlider line here. Nothing else changes.
    sliders.push_back(MakeSlider(0, "BGM Volume:", { bgm }));
    sliders.push_back(MakeSlider(1, "SFX Volume:", { se_click, se_rat }));

    for (Slider& s : sliders) {
        s.CreateMeshes();
        s.ApplyDefaultVolume(); // sync audio engine to the default value
    }
}

void Settings_Update() {
    // get mouse position and convert to world space
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    f32 mouseX = (f32)mx - AEGfxGetWinMaxX();
    f32 mouseY = -(f32)my + AEGfxGetWinMaxY();

    // only true on the first frame the mouse button is pressed
    bool justClicked = AEInputCheckTriggered(AEVK_LBUTTON);

    // update sliders while mouse is held
    if (AEInputCheckCurr(AEVK_LBUTTON)) {
        for (Slider& s : sliders)
            s.UpdateFromMouse(mouseX, mouseY, justClicked);
    }

    // go back to main menu on ESC
    if (AEInputCheckTriggered(AEVK_ESCAPE)) {
        PlayClick();
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
        s.Free();
    }
    sliders.clear();
}

void Settings_Unload() {

}