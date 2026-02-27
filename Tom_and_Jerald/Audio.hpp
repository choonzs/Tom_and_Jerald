#include "AEEngine.h"

extern AEAudio backgroundAudio;
extern AEAudio clickAudio;
extern AEAudio ratsqueakAudio;

extern AEAudioGroup bgm;
extern AEAudioGroup se;
extern bool bgmPlaying; // tracks if background audio is already playing

// Init and Free
void AudioInit();
void AudioFree();

// Individual play functions
void PlayBackgroundAudio();
void PlayClick();
void PlayRatSqueak();

// Individual stop functions
void StopBackgroundAudio();
void StopClick();
void StopRatSqueak();