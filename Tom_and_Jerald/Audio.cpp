#include "Audio.hpp"
#include "pch.hpp"

// returns true if any recognized menu key was pressed
bool IsMenuKeyTriggered()
{
    return AEInputCheckTriggered(AEVK_RETURN) ||
        AEInputCheckTriggered(AEVK_S) ||
        AEInputCheckTriggered(AEVK_E) ||
        AEInputCheckTriggered(AEVK_P) ||
        AEInputCheckTriggered(AEVK_R) ||
        AEInputCheckTriggered(AEVK_Y) ||
        AEInputCheckTriggered(AEVK_I) ||
        AEInputCheckTriggered(AEVK_N) ||
        AEInputCheckTriggered(AEVK_C) ||
        AEInputCheckTriggered(AEVK_T) ||
        AEInputCheckTriggered(AEVK_M) ||
        AEInputCheckTriggered(AEVK_H) ||
        AEInputCheckTriggered(AEVK_1) ||
        AEInputCheckTriggered(AEVK_2) ||
        AEInputCheckTriggered(AEVK_3) ||
        AEInputCheckTriggered(AEVK_ESCAPE) ||
        AEInputCheckTriggered(AEVK_LBUTTON);
}

bool AudioLoadConfig(const char* filename) {
    std::ifstream ifs(filename);

    if (!ifs) {                    
        std::cerr << "Cannot open " << filename << "\n"; // check if file opens
        return false;
    }

    std::string label;  
    std::string path;

    // reads each line — label first, then path
    ifs >> label >> path;  // reads "Background_Audio Assets/background_audio.mp3"
    backgroundAudio.LoadMusic(path.c_str());

    ifs >> label >> path;  // reads "Click_Audio Assets/click_audio.mp3"
    clickAudio.LoadSound(path.c_str());

    ifs >> label >> path;  // reads "Rat_Audio Assets/ratClick_audio.mp3"
    ratsqueakAudio.LoadSound(path.c_str());

    ifs.close();
    return true;
}

// loads a music file and marks it as looping
void AudioClip::LoadMusic(const char* filename)
{
    mAudio = AEAudioLoadMusic(filename);
    mGroup = AEAudioCreateGroup();
    mIsMusic = true;
}

// loads a sound effect file and marks it as non-looping
void AudioClip::LoadSound(const char* filename)
{
    mAudio = AEAudioLoadSound(filename);
    mGroup = AEAudioCreateGroup();
    mIsMusic = false;
}

// plays the audio clip
// music will loop forever and is guarded against restarting every frame
// sound effects play once and can be retriggered
void AudioClip::Play(float volume, float pitch)
{
    // prevent music from restarting if it is already playing
    if (mIsMusic && mIsPlaying) return;

    int loop = mIsMusic ? -1 : 0; // -1 = loop forever, 0 = play once
    AEAudioPlay(mAudio, mGroup, volume, pitch, loop);
    mIsPlaying = true;
}

// stops the audio clip and resets the flag so it can be played again
void AudioClip::Stop()
{
    AEAudioStopGroup(mGroup);
    mIsPlaying = false; 
}

AudioClip backgroundAudio;
AudioClip clickAudio;
AudioClip ratsqueakAudio;

void AudioInit()
{
    // load background music and sound effects from text file
    AudioLoadConfig("Assets/AudioConfig.txt");
}

void AudioFree()
{
    // unload all audio to prevent memory leaks
    backgroundAudio.Stop();
    clickAudio.Stop();
    ratsqueakAudio.Stop();
}
