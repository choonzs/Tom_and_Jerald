#include "AEEngine.h"

// audio assets - background music and sound effects
extern AEAudio backgroundAudio;
extern AEAudio clickAudio; 
extern AEAudio ratsqueakAudio;

// audio groups - used to control volume and pitch of each group
extern AEAudioGroup bgm;
extern AEAudioGroup se_click;
extern AEAudioGroup se_rat;

// flag to track if background audio is already playing
// prevents background music from restarting every frame
extern bool bgmPlaying; 

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