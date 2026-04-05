#include "EndMenu.h"
#include "AEEngine.h"
#include "GameStateManager.h"
#include "TextManager.h"
#include "LoadingScreen.h"
#include "PlayerStats.h"
#include <cstdio>

static AEGfxVertexList* pMesh;
static AEGfxTexture* pPanelTex;
struct EndButton {
    f32 x, y, w, h;
    bool isHovered;
};
static EndButton tryAgainBtn;
static EndButton mainMenuBtn;

void EndMenu::Init()
{
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);
    pMesh = AEGfxMeshEnd();

    tryAgainBtn = { -150.f, -200.f, 250.f, 55.f, false };
    mainMenuBtn = { 150.f, -200.f, 250.f, 55.f, false };

    pPanelTex = AEGfxTextureLoad("Assets/TEMP_Sprites/audio_panel.png");
}

void EndMenu::Update()
{
    if (!isActive) return;

    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);
    float halfW = AEGfxGetWindowWidth() * 0.5f;
    float halfH = AEGfxGetWindowHeight() * 0.5f;
    float px = mx - halfW;
    float py = halfH - my;

    auto checkHover = [&](EndButton& btn) {
        btn.isHovered = (px > btn.x - btn.w * 0.5f && px < btn.x + btn.w * 0.5f &&
            py > btn.y - btn.h * 0.5f && py < btn.y + btn.h * 0.5f);
        };

    checkHover(tryAgainBtn);
    checkHover(mainMenuBtn);

    if (tryAgainBtn.isHovered && AEInputCheckTriggered(AEVK_LBUTTON))
    {
        isActive = false;
        LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;
        GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        GameStateManager::GetInstance().gamePaused = false;

    }
    if (mainMenuBtn.isHovered && AEInputCheckTriggered(AEVK_LBUTTON))
    {
        isActive = false;
        GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::MENU);
        GameStateManager::GetInstance().gamePaused = false;
    }

    int totalSeconds = static_cast<int>(PlayerStats::GetInstance().GetTotalSeconds());
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    int timeBonus = won ? max(0, 1000 - totalSeconds) : 0;

    score = PlayerStats::GetInstance().killCount * 100
        - PlayerStats::GetInstance().deathCount * 50
        + timeBonus;
}

void EndMenu::Render()
{
    if (!isActive) return;

    AEVec2 cam;
    AEGfxGetCamPosition(&cam.x, &cam.y);

    AEMtx33 scale, trans, final, identity;
    AEMtx33Identity(&identity);

    // --- Black translucent background (full screen) ---
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetColorToMultiply(0.0f, 0.0f, 0.0f, 0.6f); // black, 60% opacity

    AEMtx33Scale(&scale, (f32)AEGfxGetWindowWidth(), (f32)AEGfxGetWindowHeight());
    AEMtx33Trans(&trans, cam.x, cam.y);
    AEMtx33Concat(&final, &trans, &scale);
    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetTransform(identity.m);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);

    // --- Panel texture (centered) ---
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxTextureSet(pPanelTex, 0, 0);

    AEMtx33Scale(&scale, 700.f, 500.f);          // panel size
    AEMtx33Trans(&trans, cam.x, cam.y);           // screen center
    AEMtx33Concat(&final, &trans, &scale);
    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

    AEGfxSetTransform(identity.m);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);

    // --- Title ---
    if (won)
        AEGfxPrint(TextManager::pFont, "YOU WIN!",
            -0.15f, 0.45f, 1.4f, 0.0f, 1.0f, 0.0f, 1.0f);
    else
        AEGfxPrint(TextManager::pFont, "YOU LOST!",
            -0.18f, 0.45f, 1.4f, 1.0f, 0.0f, 0.0f, 1.0f);


    // --- Stats ---
    char scoreText[50], timeText[50], deathText[50], killCount[50];

    int totalSeconds = static_cast<int>(PlayerStats::GetInstance().GetTotalSeconds());
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    sprintf_s(scoreText, "Score: %d", score);
    if (won)
        sprintf_s(timeText, "Time: %d:%02d", minutes, seconds);
    else
        sprintf_s(timeText, "Time: DNF");
    sprintf_s(deathText, "Deaths: %d", PlayerStats::GetInstance().deathCount);
    sprintf_s(killCount, "Kills: %d", PlayerStats::GetInstance().killCount);

    AEGfxPrint(TextManager::pFont, scoreText, -0.15f, 0.30f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, timeText, -0.15f, 0.18f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, deathText, -0.15f, 0.06f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, killCount, -0.15f, -0.06f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    // --- Buttons ---
    auto drawBtn = [&](EndButton& btn, const char* label) {
        AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetColorToAdd(0, 0, 0, 0);
        AEGfxSetColorToMultiply(
            btn.isHovered ? 0.6f : 0.3f,
            btn.isHovered ? 0.6f : 0.3f,
            btn.isHovered ? 0.6f : 0.3f, 1.0f);

        AEMtx33Scale(&scale, btn.w, btn.h);
        AEMtx33Trans(&trans, btn.x + cam.x, btn.y + cam.y);
        AEMtx33Concat(&final, &trans, &scale);
        AEGfxSetTransform(final.m);
        AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);

        AEGfxSetTransform(identity.m);
        AEGfxSetColorToMultiply(1, 1, 1, 1);
        AEGfxSetColorToAdd(0, 0, 0, 0);

        float nx = btn.x / (AEGfxGetWindowWidth() * 0.5f);
        float ny = btn.y / (AEGfxGetWindowHeight() * 0.5f);
        f32 tw, th;
        AEGfxGetPrintSize(TextManager::pFont, label, 1.0f, &tw, &th);
        AEGfxPrint(TextManager::pFont, label,
            nx - tw * 0.5f, ny - th * 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
        };

    drawBtn(tryAgainBtn, "TRY AGAIN");
    drawBtn(mainMenuBtn, "MAIN MENU");
}

void EndMenu::Free()
{
    if (pMesh)
    {
        AEGfxMeshFree(pMesh);
        pMesh = nullptr;
    }
    if (pPanelTex) { AEGfxTextureUnload(pPanelTex); pPanelTex = nullptr; }
}