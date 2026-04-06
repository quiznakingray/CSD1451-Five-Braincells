//---------------------------------------------------------
// author:    Nadyrah Tarmidi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
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
        {"mainMenu", "Assets/Sounds/mainMenu.mp3"},
        {"level", "Assets/Sounds/levelAmbience.mp3"}
    };

    // Load all music audio
    for (auto& m : musicFiles)
    {
        music[m.key] = AEAudioLoadMusic(m.path);
    }

    // Store all sfx audio
    struct SFXFile { const char* key; const char* path; };
    SFXFile sfxFiles[] = {
        {"leverSwitch", "Assets/Sounds/leverSwitch.mp3"},
        { "buttonSwitch", "Assets/Sounds/buttonSwitch.mp3" },
        { "crateLanding", "Assets/Sounds/crateLanding.mp3" },
        { "playerHurt", "Assets/Sounds/playerHurt.mp3" },
        { "playerDie", "Assets/Sounds/playerDie.mp3" },

        // WHAT WE HAVE
        { "laserOn", "Assets/Sounds/laserOn.mp3" },
        { "shieldHit", "Assets/Sounds/shieldHit.mp3" },
        { "uiButtonClick", "Assets/Sounds/uiButtonClick.mp3" },
        { "itemPickup", "Assets/Sounds/itemPickup.mp3" },
        { "enemyDie", "Assets/Sounds/enemyDie.mp3" },
        { "minibossDie", "Assets/Sounds/minibossDie.mp3" },
        { "goalEnter", "Assets/Sounds/goalEnter.mp3" },
        { "arrowHit", "Assets/Sounds/arrowHit.mp3" },
        { "gateTrigger", "Assets/Sounds/gateTrigger.mp3" },

        // WHAT WE MAY WANT
        { "walk", "Assets/Sounds/walk.mp3" },
        { "jump", "Assets/Sounds/jump.mp3" }, // DONT HAVE
        { "land", "Assets/Sounds/land.mp3" },
        { "aimBow", "Assets/Sounds/aimBow.mp3" },
        { "shootArrow", "Assets/Sounds/shootArrow.mp3" },
        { "shieldOn", "Assets/Sounds/shieldOn.mp3" }, // DONT HAVE
        { "checkpoint", "Assets/Sounds/checkpoint.mp3" },
        { "win", "Assets/Sounds/win.mp3" }, // DONT HAVE
        { "lose", "Assets/Sounds/lose.mp3" }, // DONT HAVE
        { "zap", "Assets/Sounds/zap.mp3" },
        { "wheels", "Assets/Sounds/wheelsShort.mp3" },
        { "cratePickUp", "Assets/Sounds/cratePickUp.mp3" }
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