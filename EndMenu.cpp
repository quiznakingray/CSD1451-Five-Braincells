#include "EndMenu.h"
#include "AEEngine.h"
#include "GameStateManager.h"
#include "TextManager.h"
#include <iostream>
#include <cstdio> // For sprintf_s



static AEGfxVertexList* pMesh;

void EndMenu::Init() {
    // Create a simple rectangular mesh for buttons
    AEGfxMeshStart();
    AEGfxTriAdd(-100.0f, -25.0f, 0xFFFFFFFF, 0.0f, 0.0f,
        100.0f, -25.0f, 0xFFFFFFFF, 0.0f, 0.0f,
        -100.0f, 25.0f, 0xFFFFFFFF, 0.0f, 0.0f);
    AEGfxTriAdd(100.0f, -25.0f, 0xFFFFFFFF, 0.0f, 0.0f,
        100.0f, 25.0f, 0xFFFFFFFF, 0.0f, 0.0f,
        -100.0f, 25.0f, 0xFFFFFFFF, 0.0f, 0.0f);
    pMesh = AEGfxMeshEnd();
}

void EndMenu::Update() {
    if (!isActive) return;

    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);
    // Convert to world coordinates (assuming 1600x900)
    float worldX = (float)mouseX - 800.0f;
    float worldY = 450.0f - (float)mouseY;

    // Check for "Play/Try Again" Button (Y offset: -50)
    if (worldX > -100 && worldX < 100 && worldY > -75 && worldY < -25) {
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            isActive = false;
            GameStateManager gsm;
            gsm.ChangeState(GAME_STATE_TYPE::LEVEL1); // Restart from beginning
        }
    }

    // Check for "Main Menu" Button (Y offset: -120)
    if (worldX > -100 && worldX < 100 && worldY > -145 && worldY < -95) {
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            isActive = false;
            GameStateManager gsm;
            gsm.ChangeState(GAME_STATE_TYPE::MENU);
        }
    }
}

void EndMenu::Render() {
    if (!isActive) return;

    char scoreText[50], timeText[50], deathText[50];
    sprintf_s(scoreText, "Your Score: %d", score);
    sprintf_s(timeText, "Time Taken: %.2f", timeTaken);
    sprintf_s(deathText, "Death Counter: %d", deathCounter);

    // Corrected: AEGfxPrint takes 7 arguments, removed the leading '1'
    if (isWin) {
        AEGfxPrint(TextManager::pFont, (char*)"Congrats, You Win!", -0.2f, 0.4f, 1.5f, 1.0f, 1.0f, 0.0f, 1.0f);
        AEGfxPrint(TextManager::pFont,deathText, -0.15f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        AEGfxPrint(TextManager::pFont, (char*)"You Are Dead, Yikes!", -0.2f, 0.4f, 1.5f, 1.0f, 0.0f, 0.0f, 1.0f);
    }

    AEGfxPrint(TextManager::pFont, scoreText, -0.15f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(TextManager::pFont, timeText, -0.15f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
}

void EndMenu::Free() {
    if (pMesh) AEGfxMeshFree(pMesh);
}