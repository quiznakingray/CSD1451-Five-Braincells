//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "LoadingScreen.h"
#include "AEEngine.h"
#include "FadeManager.h"

#include <string>
#include <array>

GAME_STATE_TYPE LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;

int LOADING_FRAMES = 90;
int FADE_FRAMES = 35; // duration of fade-in and fade-out in frames

AEGfxTexture* loadingTexture = nullptr;
static AEGfxVertexList* mesh = nullptr;
int frameCount = 0;
int dotFrame = 0;
std::string loadingText = "Loading";
s8 fontId = 0;

const std::array<std::string, 4> dotStates = {
    "Loading",
    "Loading.",
    "Loading..",
    "Loading..."
};

void LoadingScreen::Load()
{
    loadingTexture = AEGfxTextureLoad("Assets/Environment/bg_castle.png");
}

void LoadingScreen::Init()
{
    frameCount = 0;
    dotFrame = 0;
    loadingText = "Loading";
    fontId = AEGfxCreateFont("Assets/liberation-mono.ttf", 60);

    AEGfxMeshStart();

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);

    mesh = AEGfxMeshEnd();
    FadeManager::GetInstance().BeginFadeIn(FADE_FRAMES);
}

void LoadingScreen::Update()
{
    frameCount++;
    if (frameCount % 20 == 0)
    {
        dotFrame = (dotFrame + 1) % 4;
        loadingText = dotStates[dotFrame];
    }

    if (frameCount >= LOADING_FRAMES && !FadeManager::GetInstance().IsFading())
    {
        FadeManager::GetInstance().BeginFadeOut(targetState, FADE_FRAMES);
    }
}

void LoadingScreen::Render()
{
    AEGfxSetBackgroundColor(0.f, 0.f, 0.f);

    // Background
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetTransparency(1.0f);

    AEGfxTextureSet(loadingTexture, 0, 0);

    float w = static_cast<float>(AEGfxGetWindowWidth());
    float h = static_cast<float>(AEGfxGetWindowHeight());

    AEMtx33 scale{}, trans{}, final{};
    AEMtx33Scale(&scale, w, h);
    AEMtx33Trans(&trans, 0.f, 0.f);
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // Text
    AEMtx33 identity{};
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);

    constexpr float textX = -0.2f;
    constexpr float textY = -0.05f;

    AEGfxPrint(fontId, loadingText.c_str(), textX, textY, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

void LoadingScreen::Free()
{
    if (mesh)
    {
        AEGfxMeshFree(mesh);
        mesh = nullptr;
    }
}

void LoadingScreen::Unload()
{
    if (loadingTexture)
    {
        AEGfxTextureUnload(loadingTexture);
        loadingTexture = nullptr;
    }

    AEGfxDestroyFont(fontId);
}