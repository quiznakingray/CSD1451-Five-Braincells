#include "Credits.h"
#include "GameStateManager.h"
#include "TextManager.h"
#include "AEEngine.h"
#include "FadeManager.h"

static AEGfxVertexList* pMesh;
static AEGfxTexture* pDigipenLogo;
static AEGfxTexture* pBackground;

struct BackButton {
    f32 x, y, w, h;
    bool isHovered;
};
static BackButton backBtn;

void CreditsMenu::Credits_Init()
{
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);
    pMesh = AEGfxMeshEnd();

    pDigipenLogo = AEGfxTextureLoad("Assets/DigiPen_Singapore_WEB_RED.png");
    pBackground = AEGfxTextureLoad("Assets/Environment/bg_castle.png");
    backBtn = { 0.f, 370.f, 200.f, 55.f, false };

    FadeManager::GetInstance().BeginFadeIn();
}

void CreditsMenu::Credits_Update()
{
    FadeManager::GetInstance().Update();

    // Block button input while a fade is already in progress
    if (FadeManager::GetInstance().IsFading()) return;

    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    float halfW = AEGfxGetWindowWidth() * 0.5f;
    float halfH = AEGfxGetWindowHeight() * 0.5f;
    float px = mx - halfW;
    float py = halfH - my;

    backBtn.isHovered = (px > backBtn.x - backBtn.w * 0.5f && px < backBtn.x + backBtn.w * 0.5f &&
        py > backBtn.y - backBtn.h * 0.5f && py < backBtn.y + backBtn.h * 0.5f);

    if (backBtn.isHovered && AEInputCheckTriggered(AEVK_LBUTTON))
        next = GAME_STATE_TYPE::MENU;

    if (AEInputCheckTriggered(AEVK_ESCAPE))
        next = GAME_STATE_TYPE::MENU;
}

void CreditsMenu::Credits_Draw()
{
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    // --- Background ---
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxTextureSet(pBackground, 0, 0);

    AEMtx33 scaleBg, transBg, finalBg;
    AEMtx33Scale(&scaleBg, (f32)AEGfxGetWindowWidth(), (f32)AEGfxGetWindowHeight());
    AEMtx33Trans(&transBg, 0.f, 0.f);
    AEMtx33Concat(&finalBg, &transBg, &scaleBg);
    AEGfxSetTransform(finalBg.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

    AEMtx33 identityBg;
    AEMtx33Identity(&identityBg);
    AEGfxSetTransform(identityBg.m);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);

    // --- DigiPen Logo ---
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxTextureSet(pDigipenLogo, 0, 0);

    AEMtx33 scale, trans, final;
    AEMtx33Scale(&scale, 300.f, 100.f);
    AEMtx33Trans(&trans, 0.f, 250.f);
    AEMtx33Concat(&final, &trans, &scale);
    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

    // Reset after texture
    AEMtx33 identity;
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);

    //// --- Copyright ---
    //AEGfxPrint(TextManager::pFont, "Copyright (C) 2026 DigiPen Institute of Technology.",
    //    -0.8f, 0.85f, 0.6f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Title ---
    AEGfxPrint(TextManager::pFont, "CREDITS",
        -0.10f, 0.3f, 1.6f, 1.0f, 0.8f, 0.0f, 1.0f);

    // --- Team ---
    AEGfxPrint(TextManager::pFont, "Team Members",
        -0.6f, 0.05f, 1.2f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Choy Phui Mun",
        -0.6f, -0.10f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Nadyrah Binte Tarmidi",
        -0.6f, -0.22f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Kon Xin Yi",
        -0.6f, -0.34f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Chua Qi Jun",
        -0.6f, -0.46f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Ang Zi Jun",
        -0.6f, -0.58f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Instructors ---
    AEGfxPrint(TextManager::pFont, "Instructors",
        0.30f, 0.05f, 1.2f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Dr. Soroor",
        0.30f, -0.10f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Gerald",
        0.30f, -0.22f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Tommy",
        0.30f, -0.34f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- President ---
    AEGfxPrint(TextManager::pFont, "President",
        0.30f, -0.58f, 1.2f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Claude Comair",
        0.30f, -0.7f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Back Button ---
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetColorToMultiply(
        backBtn.isHovered ? 0.5f : 0.3f,
        backBtn.isHovered ? 0.5f : 0.3f,
        backBtn.isHovered ? 0.5f : 0.3f, 1.0f);

    AEMtx33Scale(&scale, backBtn.w, backBtn.h);
    AEMtx33Trans(&trans, backBtn.x, backBtn.y);
    AEMtx33Concat(&final, &trans, &scale);
    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

    // Reset
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);

    // Button label
    float nx = backBtn.x / (AEGfxGetWindowWidth() * 0.5f);
    float ny = backBtn.y / (AEGfxGetWindowHeight() * 0.5f);
    f32 tw, th;
    AEGfxGetPrintSize(TextManager::pFont, "BACK", 1.0f, &tw, &th);
    AEGfxPrint(TextManager::pFont, "BACK",
        nx - tw * 0.5f, ny - th * 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    // fade manager render must always be last
    FadeManager::GetInstance().Render();
}

void CreditsMenu::Credits_Free()
{
    if (pDigipenLogo)
    {
        AEGfxTextureUnload(pDigipenLogo);
        pDigipenLogo = nullptr;
    }
    AEGfxMeshFree(pMesh);
    pMesh = nullptr;
}