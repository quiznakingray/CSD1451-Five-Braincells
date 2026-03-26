#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "AEEngine.h"
#include "AEAudio.h"
#include <map>
#include <string>

static std::map<std::string, AEAudio> sfx; // stores sfx audio
static std::map<std::string, AEAudio> music; // stores music audio

class AudioManager
{
public:
    static void Init();
    static void Update();
    static void Exit();

    static void SetMasterVolume(float v);
    static void SetMusicVolume(float v);
    static void SetSFXVolume(float v);

    // Get volumes for save files
    static float GetMasterVolume();
    static float GetMusicVolume();
    static float GetSFXVolume();

    static void PlaySFX(const std::string& name);
    static void PlayMusic(const std::string& name, bool loop = true);

private:
    static AEAudioGroup masterGroup;
    static AEAudioGroup musicGroup;
    static AEAudioGroup sfxGroup;

    static float masterVolume;
    static float musicVolume;
    static float sfxVolume;

    // Get audio groups
    static AEAudioGroup GetMasterGroup();
    static AEAudioGroup GetMusicGroup();
    static AEAudioGroup GetSFXGroup();

    static void LoadAllAudio(); // load all audio files here
};

#endif