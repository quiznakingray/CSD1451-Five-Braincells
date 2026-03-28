#include "AudioManager.h"

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
    Exit();
}

void AudioManager::LoadAllAudio()
{
    // Store all music audio
    struct MusicFile { const char* key; const char* path; };
    MusicFile musicFiles[] = {
        {"menu_bgm", "Assets/Sounds/Explosion CTE01_56.1.wav"},
        //{"leverSwitch.wav", "Assets/Sounds/ex1.wav"}, // EXAMPLE, REPLACE THIS, same format for sfx
        //{"callThisFileName", "Assets/Sounds/ex2.wav"} // EXAMPLE, REPLACE THIS
    };

    // Load all music audio
    for (auto& m : musicFiles)
    {
        music[m.key] = AEAudioLoadMusic(m.path);
    }

    // Store all sfx audio
    struct SFXFile { const char* key; const char* path; };
    SFXFile sfxFiles[] = {
        {"leverSwitch", "Assets/Sounds/leverSwitch.wav"},
        { "buttonSwitch", "Assets/Sounds/buttonSwitch.wav" },
        { "crateLanding", "Assets/Sounds/crateLanding.wav" },
        { "playerHurt", "Assets/Sounds/playerHurt.wav" },
        { "playerDie", "Assets/Sounds/playerDie.wav" }
    };

    // Load all sfx audio
    for (auto& s : sfxFiles)
    {
        sfx[s.key] = AEAudioLoadSound(s.path);
    }
}

void AudioManager::Init()
{
    AEAudioInit();

    masterGroup = AEAudioCreateGroup();
    musicGroup = AEAudioCreateGroup();
    sfxGroup = AEAudioCreateGroup();

    LoadAllAudio(); // load all audio files
}

void AudioManager::Update()
{
    AEAudioUpdate();
}

void AudioManager::Exit()
{
    // Stop all audio first
    AEAudioStopGroup(masterGroup);
    AEAudioStopGroup(musicGroup);
    AEAudioStopGroup(sfxGroup);

    //// Unload all sfx audio
    //for (auto& pair : sfx)
    //{
    //    if (AEAudioIsValidAudio(pair.second))
    //        AEAudioUnloadAudio(pair.second);
    //}
    //sfx.clear(); // clear the map

    //// Unload all music audio
    //for (auto& pair : music)
    //{
    //    if (AEAudioIsValidAudio(pair.second))
    //        AEAudioUnloadAudio(pair.second);
    //}
    //music.clear(); // clear the map

    // Unload audio groups
    if (AEAudioIsValidGroup(masterGroup))
        AEAudioUnloadAudioGroup(masterGroup);

    if (AEAudioIsValidGroup(musicGroup))
        AEAudioUnloadAudioGroup(musicGroup);

    if (AEAudioIsValidGroup(sfxGroup))
        AEAudioUnloadAudioGroup(sfxGroup);

    // Shutdown audio system
    AEAudioExit();
}

void AudioManager::SetMasterVolume(float v)
{
    masterVolume = v;
    AEAudioSetGroupVolume(masterGroup, v);
}

void AudioManager::SetMusicVolume(float v)
{
    musicVolume = v;
    AEAudioSetGroupVolume(musicGroup, v * masterVolume);
}

void AudioManager::SetSFXVolume(float v)
{
    sfxVolume = v;
    AEAudioSetGroupVolume(sfxGroup, v * masterVolume);
}

float AudioManager::GetMasterVolume() { return masterVolume; }
float AudioManager::GetMusicVolume() { return musicVolume; }
float AudioManager::GetSFXVolume() { return sfxVolume; }

AEAudioGroup AudioManager::GetMasterGroup() { return masterGroup; }
AEAudioGroup AudioManager::GetMusicGroup() { return musicGroup; }
AEAudioGroup AudioManager::GetSFXGroup() { return sfxGroup; }

void AudioManager::PlaySFX(const std::string& name)
{
    if (sfx.count(name))
        AEAudioPlay(sfx[name], sfxGroup, sfxVolume * masterVolume, 1.0f, 0);
}

void AudioManager::PlayMusic(const std::string& name, bool loop)
{
    if (music.count(name))
        AEAudioPlay(music[name], musicGroup, musicVolume * masterVolume, 1.0f, loop ? -1 : 0);
}