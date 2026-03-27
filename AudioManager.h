#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include "AEEngine.h"
#include "AEAudio.h"
#include "SingletonTemplate.h"
#include <map>
#include <string>

class AudioManager : public Singleton<AudioManager>
{
public:
    void Init();
    void Update();
    void Exit();

    void SetMasterVolume(float v);
    void SetMusicVolume(float v);
    void SetSFXVolume(float v);

    // Get volumes for save files
    float GetMasterVolume();
    float GetMusicVolume();
    float GetSFXVolume();

    void PlaySFX(const std::string& name);
    void PlayMusic(const std::string& name, bool loop = true);

private:
    friend class Singleton<AudioManager>;

    AudioManager();
    ~AudioManager();

    std::map<std::string, AEAudio> sfx; // stores sfx audio
    std::map<std::string, AEAudio> music; // stores music audio

    AEAudioGroup masterGroup;
    AEAudioGroup musicGroup;
    AEAudioGroup sfxGroup;

    float masterVolume = 1.0f;
    float musicVolume = 1.0f;
    float sfxVolume = 1.0f;

    // Get audio groups
    AEAudioGroup GetMasterGroup();
    AEAudioGroup GetMusicGroup();
    AEAudioGroup GetSFXGroup();

    void LoadAllAudio(); // load all audio files here
};

#endif