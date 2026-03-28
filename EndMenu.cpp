#include "EndMenu.h"
#include "AEEngine.h"
#include "GameStateManager.h"
#include <iostream>

bool EndMenu::isActive = false;
bool EndMenu::isWin = false;
int EndMenu::score = 0;
float EndMenu::timeTaken = 0.0f;
int EndMenu::deathCounter = 0;

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
            gsm.ChangeState(GAME_STATE_TYPE::WORLD); // Restart from beginning
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

    // 1. Draw a semi-transparent dark background overlay
    // (Optional: Draw a large black mesh here)

    // 2. Draw Text
    char scoreText[50], timeText[50], deathText[50];
    sprintf_s(scoreText, "Your Score: %d", score);
    sprintf_s(timeText, "Time Taken: %.2f", timeTaken);
    sprintf_s(deathText, "Death Counter: %d", deathCounter);

    if (isWin) {
        AEGfxPrint(1, (char*)"Congrats, You Win!", -0.2f, 0.4f, 1.5f, 1.0f, 1.0f, 0.0f);
        AEGfxPrint(1, deathText, -0.15f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f);
    }
    else {
        AEGfxPrint(1, (char*)"You Are Dead, Yikes!", -0.2f, 0.4f, 1.5f, 1.0f, 0.0f, 0.0f);
    }

    AEGfxPrint(1, scoreText, -0.15f, 0.1f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(1, timeText, -0.15f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f);

    // 3. Draw Buttons
    AEMtx33 trans;
    AEMtx33Trans(&trans, 0, -50);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxPrint(1, isWin ? (char*)"Play Again" : (char*)"Try Again", -0.05f, -0.12f, 1.0f, 0.0f, 0.0f, 0.0f);

    AEMtx33Trans(&trans, 0, -120);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxPrint(1, (char*)"Main Menu", -0.05f, -0.27f, 1.0f, 0.0f, 0.0f, 0.0f);
}

void EndMenu::Free() {
    if (pMesh) AEGfxMeshFree(pMesh);
}