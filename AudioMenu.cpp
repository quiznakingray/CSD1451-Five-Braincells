#include "AudioMenu.h"
#include "CameraSystem.h"
#include "SaveManager.h"
#include <iostream>

void AudioMenu::Toggle()
{
    isOpen = !isOpen;
    if (!isOpen) {
        SaveManager::GetInstance().SaveAudioData();
    }
}

bool AudioMenu::IsOpen() const
{
    return isOpen;
}

float AudioMenu::NormalizeScreenX(float px) const
{
    return px / (AEGfxGetWindowWidth() * 0.5f);
}

float AudioMenu::NormalizeScreenY(float py) const
{
    return py / (AEGfxGetWindowHeight() * 0.5f);
}

void AudioMenu::ApplyAudio()
{
    float master = masterSlider.value / 100.0f;
    float music = musicSlider.value / 100.0f;
    float sfx = sfxSlider.value / 100.0f;

    AudioManager::GetInstance().SetMasterVolume(master);
    AudioManager::GetInstance().SetMusicVolume(music);
    AudioManager::GetInstance().SetSFXVolume(sfx);
}

void AudioMenu::Init()
{
    // Font
    font = AEGfxCreateFont("Assets/liberation-mono.ttf", static_cast<int>(fontSize));

    //float screenW = (float)AEGfxGetWindowWidth();
    //float screenH = (float)AEGfxGetWindowHeight();

    panelTex = AEGfxTextureLoad("Assets/TEMP_Sprites/audio_panel.png");

    // Mesh
    AEGfxMeshStart();

    AEGfxTriAdd(-0.5f, -0.5f, 0xFF000000, 0, 1,
        0.5f, -0.5f, 0xFF000000, 1, 1,
        0.5f, 0.5f, 0xFF000000, 1, 0);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFF000000, 0, 1,
        0.5f, 0.5f, 0xFF000000, 1, 0,
        -0.5f, 0.5f, 0xFF000000, 0, 0);

    panelMesh = AEGfxMeshEnd();

    // Button pos and sizes
    float btnX = panelX;
    float btnY = panelH * -0.4f;

    float btnWidth = panelW * 0.4f;
    float btnHeight = 70.0f;

    // Slider pos and sizes
    float sliderWidth = panelW * 0.8f;
    float sliderHeight = 50.0f;

    float startY = panelH * 0.2f;
    float spacing = 130.0f;

    masterPos = startY;
    musicPos = startY - spacing;
    sfxPos = startY - spacing * 2;

    // Initialize back button
    backButton.Init(btnX, btnY, btnWidth, btnHeight,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png"
    );

    // Initialize audio sliders
    const char* bar = "Assets/TEMP_Sprites/slider_bar.png";
    const char* handle = "Assets/TEMP_Sprites/slider_button.png";

    if (SaveManager::GetInstance().HasSaveData()) {
        SaveManager::GetInstance().LoadAudioData();
    }

    masterSlider.Init(0.0f, masterPos, sliderWidth, sliderHeight, 0, 100, AudioManager::GetInstance().GetMasterVolume() * 100, bar, handle);
    musicSlider.Init(0.0f, musicPos, sliderWidth, sliderHeight, 0, 100, AudioManager::GetInstance().GetMusicVolume() * 100, bar, handle);
    sfxSlider.Init(0.0f, sfxPos, sliderWidth, sliderHeight, 0, 100, AudioManager::GetInstance().GetSFXVolume() * 100, bar, handle);

    float offsetX = sliderWidth * 0.57f; // distance from slider center

    // Master +/- button
    masterMinus.Init(-offsetX, masterPos, btnSize, btnSize,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png");

    masterPlus.Init(offsetX, masterPos, btnSize, btnSize,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png");

    // Music +/- button
    musicMinus.Init(-offsetX, musicPos, btnSize, btnSize,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png");

    musicPlus.Init(offsetX, musicPos, btnSize, btnSize,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png");

    // Sfx +/- button
    sfxMinus.Init(-offsetX, sfxPos, btnSize, btnSize,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png");

    sfxPlus.Init(offsetX, sfxPos, btnSize, btnSize,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png");
}

void AudioMenu::Update()
{
    panelX = CameraSystem::GetCameraPos().x;
    panelY = CameraSystem::GetCameraPos().y;

    backButton.Update(panelX, panelY + panelH * -0.4f);


    // Close audio panel if back button clicked
    if (backButton.IsClicked() && IsOpen())
        Toggle();

    masterSlider.Update(panelX, panelY + masterPos);
    musicSlider.Update(panelX, panelY + musicPos);
    sfxSlider.Update(panelX, panelY + sfxPos);

    // Only update audio when values change
    static int prevMaster = -1;
    static int prevMusic = -1;
    static int prevSFX = -1;

    int currMaster = (int)masterSlider.value;
    int currMusic = (int)musicSlider.value;
    int currSFX = (int)sfxSlider.value;

    // Update master volume based on master slider
    if (currMaster != prevMaster)
    {
        AudioManager::GetInstance().SetMasterVolume(currMaster / 100.0f);

        // Update music and sfx volumes based on master slider
        AudioManager::GetInstance().SetMusicVolume(currMusic / 100.0f);
        AudioManager::GetInstance().SetSFXVolume(currSFX / 100.0f);

        prevMaster = currMaster;
    }

    // Update music volume based on music slider
    if (currMusic != prevMusic)
    {
        AudioManager::GetInstance().GetInstance().SetMusicVolume(currMusic / 100.0f);
        prevMusic = currMusic; 
    }

    // Update sfx volume based on sfx slider
    if (currSFX != prevSFX)
    {
        AudioManager::GetInstance().SetSFXVolume(currSFX / 100.0f);
        prevSFX = currSFX;
    }

    masterMinus.Update(panelX - panelW * 0.8f * 0.57f, panelY + masterPos);
    masterPlus.Update(panelX + panelW * 0.8f * 0.57f, panelY + masterPos);
    musicMinus.Update(panelX - panelW * 0.8f * 0.57f, panelY + musicPos);
    musicPlus.Update(panelX + panelW * 0.8f * 0.57f, panelY + musicPos);
    sfxMinus.Update(panelX - panelW * 0.8f * 0.57f, panelY + sfxPos);
    sfxPlus.Update(panelX + panelW * 0.8f * 0.57f, panelY + sfxPos);

    // Update master volume based on master +/- button
    if (masterMinus.IsClicked())
    {
        masterSlider.value = AEClamp(masterSlider.value - 1.0f, 0.0f, 100.0f);
    }
    if (masterPlus.IsClicked())
    {
        masterSlider.value = AEClamp(masterSlider.value + 1.0f, 0.0f, 100.0f);
    }

    // Update music volume based on music +/- button
    if (musicMinus.IsClicked())
    {
        musicSlider.value = AEClamp(musicSlider.value - 1.0f, 0.0f, 100.0f);
    }
    if (musicPlus.IsClicked())
    {
        musicSlider.value = AEClamp(musicSlider.value + 1.0f, 0.0f, 100.0f);
    }

    // Update sfx volume based on sfx +/- button
    if (sfxMinus.IsClicked())
    {
        sfxSlider.value = AEClamp(sfxSlider.value - 1.0f, 0.0f, 100.0f);
    }
    if (sfxPlus.IsClicked())
    {
        sfxSlider.value = AEClamp(sfxSlider.value + 1.0f, 0.0f, 100.0f);
    }
}

void AudioMenu::Render()
{
    if (IsOpen())
    {
        // MUST RESET EVERYTHING FIRST
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetColorToMultiply(1, 1, 1, 1);
        AEGfxSetColorToAdd(0, 0, 0, 0);
        AEGfxSetTransparency(1.0f);


        AEMtx33 scaleBg, transBg, finalBg;
        AEMtx33Scale(&scaleBg, (float)AEGfxGetWindowWidth(), (float)AEGfxGetWindowHeight());
        AEMtx33Trans(&transBg, panelX, panelY);
        AEMtx33Concat(&finalBg, &transBg, &scaleBg);

        AEGfxSetTransparency(0.5f);
        AEGfxSetTransform(finalBg.m);
        AEGfxMeshDraw(panelMesh, AE_GFX_MDM_TRIANGLES);

        // Audio panel

        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
        AEGfxSetTransparency(1.0f);
        AEMtx33 scale, trans, final;
        AEMtx33Scale(&scale, panelW, panelH);
        AEMtx33Trans(&trans, panelX, panelY);
        AEMtx33Concat(&final, &trans, &scale);

        AEGfxSetTransform(final.m);
        AEGfxTextureSet(panelTex, 0, 0);
        AEGfxMeshDraw(panelMesh, AE_GFX_MDM_TRIANGLES);

        // RESET
        AEMtx33 identity;
        AEMtx33Identity(&identity);
        AEGfxSetTransform(identity.m);

        AEGfxSetColorToAdd(0, 0, 0, 0);
        AEGfxSetColorToMultiply(1, 1, 1, 1);

        // Back button and sliders
        backButton.Render();
        masterSlider.Render();
        musicSlider.Render();
        sfxSlider.Render();

        // Increase / Decrease buttons
        masterMinus.Render();
        masterPlus.Render();

        musicMinus.Render();
        musicPlus.Render();

        sfxMinus.Render();
        sfxPlus.Render();

        // Master +/- text
        AEGfxPrint(font, "-", NormalizeScreenX(-panelW * 0.47f), NormalizeScreenY(masterPos + -10.7f), fontSize * 0.006f, 1, 1, 1, 1);
        AEGfxPrint(font, "+", NormalizeScreenX(panelW * 0.44f), NormalizeScreenY(masterPos + -10.7f), fontSize * 0.006f, 1, 1, 1, 1);

        // Music +/- text
        AEGfxPrint(font, "-", NormalizeScreenX(-panelW * 0.47f), NormalizeScreenY(musicPos + -10.7f), fontSize * 0.006f, 1, 1, 1, 1);
        AEGfxPrint(font, "+", NormalizeScreenX(panelW * 0.44f), NormalizeScreenY(musicPos + -10.7f), fontSize * 0.006f, 1, 1, 1, 1);

        // Sfx +/- text
        AEGfxPrint(font, "-", NormalizeScreenX(-panelW * 0.47f), NormalizeScreenY(sfxPos + -10.7f), fontSize * 0.006f, 1, 1, 1, 1);
        AEGfxPrint(font, "+", NormalizeScreenX(panelW * 0.44f), NormalizeScreenY(sfxPos + -10.7f), fontSize * 0.006f, 1, 1, 1, 1);

        // Back button text
        float backTextX = NormalizeScreenX(-40.0f); // button center X
        float backTextY = NormalizeScreenY(panelH * -0.42f); // button center Y
        AEGfxPrint(font, "Back", backTextX, backTextY, fontSize * 0.006f, 1, 1, 1, 1);

        // Top center title
        float titleY = panelH * 0.4f; // top of panel
        float nx = NormalizeScreenX(- 70.0f);
        float ny = NormalizeScreenY(titleY);
        AEGfxPrint(font, "AUDIO", nx, ny, fontSize * 0.007f, 1, 1, 1, 1);

        // Slider labels
        float sliderOffsetX = -panelW * 0.4f; // slightly left
        float sliderOffsetY = 20.0f; // above slider

        // Master
        float masterLabelX = NormalizeScreenX(sliderOffsetX);
        float masterLabelY = NormalizeScreenY(masterPos + sliderOffsetY);
        AEGfxPrint(font, "Master", masterLabelX, masterLabelY, fontSize * 0.006f, 1, 1, 1, 1);

        // Music
        float musicLabelX = NormalizeScreenX(sliderOffsetX);
        float musicLabelY = NormalizeScreenY(musicPos + sliderOffsetY);
        AEGfxPrint(font, "Music", musicLabelX, musicLabelY, fontSize * 0.006f, 1, 1, 1, 1);

        // Sfx
        float sfxLabelX = NormalizeScreenX(sliderOffsetX);
        float sfxLabelY = NormalizeScreenY(sfxPos + sliderOffsetY);
        AEGfxPrint(font, "Sfx", sfxLabelX, sfxLabelY, fontSize * 0.006f, 1, 1, 1, 1);
    }
}

void AudioMenu::Free()
{
    backButton.Free();

    masterSlider.Free();
    musicSlider.Free();
    sfxSlider.Free();

    masterMinus.Free();
    masterPlus.Free();
    musicMinus.Free();
    musicPlus.Free();
    sfxMinus.Free();
    sfxPlus.Free();

    if (panelTex)
    {
        AEGfxTextureUnload(panelTex);
        panelTex = nullptr;
    }

    if (panelMesh)
    {
        AEGfxMeshFree(panelMesh);
        panelMesh = nullptr;
    }
}