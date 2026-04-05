//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "SplashScreen.h"
#include "AEEngine.h"
#include "GameStateManager.h"

static AEGfxTexture* splashTexture = nullptr;
static AEGfxVertexList* mesh = nullptr;
static float timer = 0.0f;

static const float SPLASH_DURATION = 3.0f;
static float alpha = 1.0f;

static const float SPLASH_HOLD = 2.0f; 
static const float SPLASH_FADEOUT = 1.0f; 

void SplashScreen::Load()
{
    splashTexture = AEGfxTextureLoad("Assets/DigiPen_BLACK.png");
}

void SplashScreen::Init()
{
    timer = 0.0f;
    alpha = 1.0f;

    AEGfxMeshStart();

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);

    mesh = AEGfxMeshEnd();
}

void SplashScreen::Update()
{
    double dt = AEFrameRateControllerGetFrameTime();
    timer += static_cast<float>(dt);

    // Start fading out after hold duration
    if (timer >= SPLASH_HOLD)
    {
        float fadeProgress = (timer - SPLASH_HOLD) / SPLASH_FADEOUT;
        alpha = 1.0f - fadeProgress;

        if (alpha <= 0.0f)
        {
            alpha = 0.0f;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::MENU);
        }
    }

    if (AEInputCheckTriggered(AEVK_RETURN) || AEInputCheckTriggered(AEVK_SPACE))
        GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::MENU);
}

void SplashScreen::Render()
{
    AEGfxSetBackgroundColor(0.f, 0.f, 0.f);

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(1, 1, 1, alpha); 
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetTransparency(alpha);

    AEGfxTextureSet(splashTexture, 0, 0);

    float winW = static_cast<float>(AEGfxGetWindowWidth());
    float winH = static_cast<float>(AEGfxGetWindowHeight());

    const float imgW = 2362.f;
    const float imgH = 945.f;
    const float imgAspect = imgW / imgH;
    const float winAspect = winW / winH;

    float drawW, drawH;
    if (winAspect > imgAspect)
    {
        drawH = winH;
        drawW = drawH * imgAspect;
    }
    else
    {
        drawW = winW;
        drawH = drawW / imgAspect;
    }

    // Scale down to half size
    drawW *= 0.5f;
    drawH *= 0.5f;

    AEMtx33 scale{}, trans{}, final{};
    AEMtx33Scale(&scale, drawW, drawH);
    AEMtx33Trans(&trans, 0.f, 0.f); 
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    AEMtx33 identity{};
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);
}

void SplashScreen::Free()
{
    if (mesh)
    {
        AEGfxMeshFree(mesh);
        mesh = nullptr;
    }
}

void SplashScreen::Unload()
{
    if (splashTexture)
    {
        AEGfxTextureUnload(splashTexture);
        splashTexture = nullptr;
    }
}