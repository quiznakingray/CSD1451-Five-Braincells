//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "FadeManager.h"

void FadeManager::BuildFadeQuad()
{
    if (fadeQuadMesh) return;
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);
    fadeQuadMesh = AEGfxMeshEnd();
}

void FadeManager::DrawOverlay(float alpha)
{
    if (!fadeQuadMesh || alpha <= 0.f) return;

    const float w = static_cast<float>(AEGfxGetWindowWidth());
    const float h = static_cast<float>(AEGfxGetWindowHeight());

    float camX = 0.0f, camY = 0.0f;
    AEGfxGetCamPosition(&camX, &camY);

    AEMtx33 scale{}, trans{}, final{};
    AEMtx33Scale(&scale, w, h);

    // lock the fade quad to the camera's position, not world origin
    AEMtx33Trans(&trans, camX, camY);
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(0.f, 0.f, 0.f, alpha);
    AEGfxSetColorToAdd(0.f, 0.f, 0.f, 0.f);
    AEGfxSetTransparency(alpha);
    AEGfxTextureSet(nullptr, 0, 0);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(fadeQuadMesh, AE_GFX_MDM_TRIANGLES);
}

void FadeManager::BeginFadeOut(GAME_STATE_TYPE target, int frames)
{
    if (fadeOutActive) return;
    BuildFadeQuad();
    fadeOutActive = true;
    fadeOutFrame = 0;
    fadeOutFrames = frames;
    fadeTarget = target;
}

void FadeManager::BeginFadeIn(int frames)
{
    BuildFadeQuad();
    fadeInActive = true;
    fadeInFrame = 0;
    fadeInFrames = frames;
    holdBlack = false; // fade-in takes over from holdBlack
}

void FadeManager::Update()
{
    // Fade-out tick
    if (fadeOutActive)
    {
        ++fadeOutFrame;
        if (fadeOutFrame >= fadeOutFrames)
        {
            fadeOutActive = false;
            fadeOutFrame = 0;
            // Stay black until the next state explicitly calls BeginFadeIn
            holdBlack = true;
            GameStateManager::GetInstance().ChangeState(fadeTarget);
        }
    }

    // Fade-in tick
    if (fadeInActive)
    {
        // Once fading in, no longer "holding" a solid black screen
        holdBlack = false;
        ++fadeInFrame;
        if (fadeInFrame >= fadeInFrames)
        {
            fadeInActive = false;
            fadeInFrame = 0;
        }
    }
}

void FadeManager::Render()
{
    // if actively fading in, draw the decreasing alpha
    if (fadeInActive)
    {
        const float alpha = 1.f - (static_cast<float>(fadeInFrame) / static_cast<float>(fadeInFrames));
        DrawOverlay(alpha);
    }
    // if actively fading out, draw the increasing alpha
    else if (fadeOutActive)
    {
        const float alpha = static_cast<float>(fadeOutFrame) / static_cast<float>(fadeOutFrames);
        DrawOverlay(alpha);
    }
    // If just finished a fade-out, keep the screen black 
    // until the new state is ready to fade in
    else if (holdBlack)
    {
        DrawOverlay(1.f);
    }
}

FadeManager::~FadeManager()
{
    if (fadeQuadMesh)
    {
        AEGfxMeshFree(fadeQuadMesh);
        fadeQuadMesh = nullptr;
    }
}