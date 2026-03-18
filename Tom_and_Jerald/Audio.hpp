#pragma once
#include "AEEngine.h"

class AudioClip {
    public:
        //AudioClip() = default;

        // import audio as music (loops) or sound effect (plays once) from external file
        void LoadMusic(const char* filename);
        void LoadSound(const char* filename);

        // play the audio clip with optional volume and pitch
        void Play(float volume = 1.f, float pitch = 1.f);
        void Stop();

    private:
        AEAudio      mAudio; // the audio asset
        AEAudioGroup mGroup; // the audio group for volume/pitch control
        bool         mIsMusic = false; // true = music (loops), false = sound effect (plays once)
        bool         mIsPlaying = false; // tracks if audio is currently playing
};

// global audio objects accessible across all game states
extern AudioClip backgroundAudio; // background music
extern AudioClip clickAudio; // UI button click sound effect
extern AudioClip ratsqueakAudio; // rat squeak sound effect during game

// returns true if any menu navigation key was triggered this frame
// used to play click sound without copy pasting in every game state
bool IsMenuKeyTriggered();

bool AudioLoadConfig(const char* filename);

// init and Free
void AudioInit();
void AudioFree();