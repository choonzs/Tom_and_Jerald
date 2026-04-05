/*************************************************************************
@file    Audio.hpp
@Author  Ong Jin Ting o.jinting@digipen.edu
@Co-authors  NIL
@brief
     Declares the AudioClip wrapper and AudioInit/AudioFree functions
     for FMOD-based background music and sound-effect playback.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
#pragma once
#include "pch.hpp"

class AudioClip {
    public:
        //AudioClip() = default;
        AEAudioGroup GetGroup() const;

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