#include "Controls.h"
#include "GameStateManager.h"
#include "TextManager.h"
#include "AEEngine.h"

static AEGfxVertexList* pMesh;
static AEGfxTexture* pBackground;

struct BackButton {
    f32 x, y, w, h;
    bool isHovered;
};
static BackButton backBtn;

void ControlsMenu::Controls_Init()
{
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);
    pMesh = AEGfxMeshEnd();

    pBackground = AEGfxTextureLoad("Assets/Environment/bg_castle.png"); // change path

    backBtn = { 0.f, 390.f, 200.f, 55.f, false };
}

void ControlsMenu::Controls_Update()
{
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

void ControlsMenu::Controls_Draw()
{
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    // --- Background ---
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxTextureSet(pBackground, 0, 0);

    AEMtx33 scale, trans, final;
    AEMtx33Scale(&scale, (f32)AEGfxGetWindowWidth(), (f32)AEGfxGetWindowHeight());
    AEMtx33Trans(&trans, 0.f, 0.f);
    AEMtx33Concat(&final, &trans, &scale);
    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

    AEMtx33 identity;
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);

    // --- Title ---
    AEGfxPrint(TextManager::pFont, "CONTROLS",
        -0.15f, 0.65f, 1.6f, 1.0f, 0.8f, 0.0f, 1.0f);

    // --- Movement ---
    AEGfxPrint(TextManager::pFont, "MOVEMENT",
        -0.90f, 0.43f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[A]",
        -0.90f, 0.29f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Move Left",
        -0.65f, 0.29f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[D]",
        -0.90f, 0.17f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Move Right",
        -0.65f, 0.17f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[SPACE]",
        -0.90f, 0.05f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Jump",
        -0.65f, 0.05f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Player ---
    AEGfxPrint(TextManager::pFont, "PLAYER",
        -0.90f, -0.13f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[E]",
        -0.90f, -0.25f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Switch to Shield Player",
        -0.65f, -0.25f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[R]",
        -0.90f, -0.37f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Switch to Range Player",
        -0.65f, -0.37f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Abilities ---
    AEGfxPrint(TextManager::pFont, "ABILITIES",
        -0.90f, -0.55f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[Q]",
        -0.90f, -0.67f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Use Ability",
        -0.65f, -0.67f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[LMB]",
        -0.90f, -0.79f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Shoot Arrow (Range only)",
        -0.65f, -0.79f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Misc ---
    AEGfxPrint(TextManager::pFont, "MISC",
        0.20f, 0.43f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[F]",
        0.20f, 0.29f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Interact",
        0.45f, 0.29f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "[ESC]",
        0.20f, 0.17f, 0.9f, 1.0f, 1.0f, 0.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, "Pause",
        0.45f, 0.17f, 0.9f, 1.0f, 1.0f, 1.0f, 1.0f);

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
}

void ControlsMenu::Controls_Free()
{
    if (pBackground)
    {
        AEGfxTextureUnload(pBackground);
        pBackground = nullptr;
    }
    AEGfxMeshFree(pMesh);
    pMesh = nullptr;
}