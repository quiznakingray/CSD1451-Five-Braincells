#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager()
    : masterGroup{}
    , musicGroup{}
    , sfxGroup{}
{
}

AudioManager::~AudioManager()
{
    //Exit();
}

void AudioManager::LoadAllAudio()
{
    // Store all music audio
    struct MusicFile { const char* key; const char* path; };
    MusicFile musicFiles[] = {
        {"mainMenu", "Assets/Sounds/mainMenu.wav"},
        {"level", "Assets/Sounds/levelAmbience.wav"}
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
        { "playerDie", "Assets/Sounds/playerDie.wav" },

        // WHAT WE HAVE
        { "laserOn", "Assets/Sounds/laserOn.wav" },
        { "shieldHit", "Assets/Sounds/shieldHit.wav" },
        { "uiButtonClick", "Assets/Sounds/uiButtonClick.wav" },
        { "itemPickup", "Assets/Sounds/itemPickup.wav" },
        { "enemyDie", "Assets/Sounds/enemyDie.wav" },
        { "minibossDie", "Assets/Sounds/minibossDie.wav" },
        { "goalEnter", "Assets/Sounds/goalEnter.wav" },
        { "arrowHit", "Assets/Sounds/arrowHit.wav" },
        { "gateTrigger", "Assets/Sounds/gateTrigger.wav" },

        // WHAT WE MAY WANT
        { "walk", "Assets/Sounds/walk.wav" },
        { "jump", "Assets/Sounds/jump.wav" }, // DONT HAVE
        { "land", "Assets/Sounds/land.wav" },
        { "aimBow", "Assets/Sounds/aimBow.wav" },
        { "shootArrow", "Assets/Sounds/shootArrow.wav" },
        { "shieldOn", "Assets/Sounds/shieldOn.wav" }, // DONT HAVE
        { "checkpoint", "Assets/Sounds/checkpoint.wav" },
        { "win", "Assets/Sounds/win.wav" }, // DONT HAVE
        { "lose", "Assets/Sounds/lose.wav" }, // DONT HAVE
        { "zap", "Assets/Sounds/zap.wav" },
        { "wheels", "Assets/Sounds/wheelsShort.wav" },
        { "cratePickUp", "Assets/Sounds/cratePickUp.wav" }
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

    //SetMasterVolume(GetMasterVolume());
    //SetMusicVolume(GetMusicVolume());
    //SetSFXVolume(GetSFXVolume());
}

void AudioManager::Update()
{
    AEAudioUpdate();
}

void AudioManager::Exit()
{
    // Stop all audio first
    if (AEAudioIsValidGroup(masterGroup))
        AEAudioStopGroup(masterGroup);

    if (AEAudioIsValidGroup(musicGroup))
        AEAudioStopGroup(musicGroup);

    if (AEAudioIsValidGroup(sfxGroup))
        AEAudioStopGroup(sfxGroup);

    // Unload all individual music audio
    for (auto& m : music)
    {
        if (AEAudioIsValidAudio(m.second))
            AEAudioUnloadAudio(m.second);
    }

    // Unload all individual sfx audio
    for (auto& s : sfx)
    {
        if (AEAudioIsValidAudio(s.second))
            AEAudioUnloadAudio(s.second);
    }

    // Unload the audio groups
    if (AEAudioIsValidGroup(masterGroup))
        AEAudioUnloadAudioGroup(masterGroup);

    if (AEAudioIsValidGroup(musicGroup))
        AEAudioUnloadAudioGroup(musicGroup);

    if (AEAudioIsValidGroup(sfxGroup))
        AEAudioUnloadAudioGroup(sfxGroup);

    // Shutdown audio system
    AEAudioExit();

    music.clear();
    sfx.clear();
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

float AudioManager::GetMasterVolume() const { return masterVolume; }
float AudioManager::GetMusicVolume() const { return musicVolume; }
float AudioManager::GetSFXVolume() const { return sfxVolume; }

AEAudioGroup AudioManager::GetMasterGroup() const { return masterGroup; }
AEAudioGroup AudioManager::GetMusicGroup() const { return musicGroup; }
AEAudioGroup AudioManager::GetSFXGroup() const { return sfxGroup; }

void AudioManager::PlaySFX(const std::string& name)
{
    if (sfx.count(name))
        AEAudioPlay(sfx[name], sfxGroup, sfxVolume * masterVolume, 1.0f, 0);
    std::cout << name << '\n';
}

void AudioManager::PlayMusic(const std::string& name, bool loop)
{
    // If same music already playing, do nothing
    if (currentMusic == name)
        return;

    // Stop all currently playing music in the music group
    if (AEAudioIsValidGroup(musicGroup))
        AEAudioStopGroup(musicGroup);

    // Play new music
    if (music.count(name))
    {
        AEAudioPlay(music[name], musicGroup, musicVolume * masterVolume, 1.0f, loop ? -1 : 0);
        currentMusic = name;
    }
}