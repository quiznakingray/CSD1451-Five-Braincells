#include "AudioMenu.h"

void AudioMenu::ApplyAudio()
{
    float master = masterSlider.value / 100.0f;
    float music = musicSlider.value / 100.0f;
    float sfx = sfxSlider.value / 100.0f;

    AudioManager::GetInstance().GetInstance().SetMasterVolume(master);
    AudioManager::GetInstance().GetInstance().SetMusicVolume(music);
    AudioManager::GetInstance().GetInstance().SetSFXVolume(sfx);
}

void AudioMenu::Init()
{
    float screenW = (float)AEGfxGetWindowWidth();
    float screenH = (float)AEGfxGetWindowHeight();

    // Normalized screen center (0,0)
    panelX = 0.0f;
    panelY = 0.0f;

    panelW = 700.0f; // width in normalized space
    panelH = 700.0f; // height in normalized space

    panelTex = AEGfxTextureLoad("Assets/TEMP_Sprites/audio_panel.png");

    // Mesh
    AEGfxMeshStart();

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);

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
    float spacing = 100.0f;

    // Initialize back button
    backButton.Init(btnX, btnY, btnWidth, btnHeight,
        "Assets/TEMP_Sprites/button_idle.png",
        "Assets/TEMP_Sprites/button_hover.png"
    );

    // Initialize audio sliders
    const char* bar = "Assets/TEMP_Sprites/slider_bar.png";
    const char* handle = "Assets/TEMP_Sprites/slider_button.png";

    masterSlider.Init(0.0f, startY, sliderWidth, sliderHeight, 0, 100, 100, bar, handle);
    musicSlider.Init(0.0f, startY - spacing, sliderWidth, sliderHeight, 0, 100, 100, bar, handle);
    sfxSlider.Init(0.0f, startY - spacing * 2, sliderWidth, sliderHeight, 0, 100, 100, bar, handle);
}

void AudioMenu::Update()
{
    backButton.Update();

    masterSlider.Update();
    musicSlider.Update();
    sfxSlider.Update();

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
        AudioManager::GetInstance().GetInstance().SetMasterVolume(currMaster / 100.0f);

        // Update music and sfx volumes based on master slider
        AudioManager::GetInstance().GetInstance().SetMusicVolume(currMusic / 100.0f);
        AudioManager::GetInstance().GetInstance().SetSFXVolume(currSFX / 100.0f);

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
        AudioManager::GetInstance().GetInstance().SetSFXVolume(currSFX / 100.0f);
        prevSFX = currSFX;
    }
}

void AudioMenu::Render()
{
    // MUST RESET EVERYTHING FIRST
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetTransparency(1.0f);

    // Audio panel
    AEGfxTextureSet(panelTex, 0, 0);

    AEMtx33 scale, trans, final;
    AEMtx33Scale(&scale, panelW, panelH);
    AEMtx33Trans(&trans, panelX, panelY);
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(panelMesh, AE_GFX_MDM_TRIANGLES);

    // RESET
    AEMtx33 identity;
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);

    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetColorToMultiply(1, 1, 1, 1);

    // Button and sliders
    backButton.Render();
    masterSlider.Render();
    musicSlider.Render();
    sfxSlider.Render();
}

void AudioMenu::Free()
{
    backButton.Free();

    masterSlider.Free();
    musicSlider.Free();
    sfxSlider.Free();

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