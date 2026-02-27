#include "Audio.hpp"
#include "AEEngine.h"

AEAudio backgroundAudio;
AEAudio clickAudio;
AEAudio ratsqueakAudio;

AEAudioGroup bgm;
AEAudioGroup se_click;
AEAudioGroup se_rat;

void AudioInit()
{
    backgroundAudio = AEAudioLoadMusic("Assets/background_audio.mp3");
    clickAudio = AEAudioLoadSound("Assets/click_audio.mp3");
    ratsqueakAudio = AEAudioLoadSound("Assets/ratClick_audio.mp3");

    bgm = AEAudioCreateGroup();
    se_click = AEAudioCreateGroup();
    se_rat = AEAudioCreateGroup();
}

void AudioFree()
{
    AEAudioUnloadAudioGroup(bgm);
    AEAudioUnloadAudioGroup(se_click);
    AEAudioUnloadAudioGroup(se_rat);
}

bool bgmPlaying = false;
void PlayBackgroundAudio()
{
    // only play if it is not already playing
    if (!bgmPlaying)
    {
        AEAudioPlay(backgroundAudio, bgm, 1.f, 1.f, -1); // loops forever
        bgmPlaying = true; // mark as playing so it won't play again
    }
}

void PlayClick()
{
    AEAudioPlay(clickAudio, se_click, 1.f, 1.f, 0); // play once
}

void PlayRatSqueak()
{
    AEAudioPlay(ratsqueakAudio, se_rat, 1.f, 1.f, 0); // play once
}

void StopBackgroundAudio()
{
    AEAudioStopGroup(bgm);
    bgmPlaying = false; // reset the flag so it can play again when called
}

void StopClick()
{
    AEAudioStopGroup(se_click);
}

void StopRatSqueak()
{
    AEAudioStopGroup(se_rat);
}