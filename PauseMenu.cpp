#include "TextManager.h"
#include "PauseMenu.h"
#include "GameStateManager.h"
#include "AEEngine.h"
#include "ParkourLevel.h"
#include "BossLevel.h"
#include <string>

// Logic flags
static bool isRestartConfirm = false;
static AEGfxVertexList* pButtonMesh = nullptr;

bool IsButtonClicked(float btnX, float btnY, float btnWidth, float btnHeight) {

    s32 screenMouseX, screenMouseY;

    AEInputGetCursorPosition(&screenMouseX, &screenMouseY);

    // Alpha Engine screen coordinates: (0,0) is top-left.
    // We transform them to world coordinates: (0,0) is center.
    float mouseX = (float)screenMouseX - (AEGfxGetWindowWidth() / 2.0f);
    float mouseY = (float)(AEGfxGetWindowHeight() / 2.0f) - screenMouseY;

    // Convert screen space to world space if necessary,

    // but menus usually use screen-space coordinates.

    // Assuming buttons are centered:

    float halfW = btnWidth / 2.0f;

    float halfH = btnHeight / 2.0f;



    if (mouseX >= (btnX - halfW) && mouseX <= (btnX + halfW) &&

        mouseY >= (btnY - halfH) && mouseY <= (btnY + halfH)) {


        return AEInputCheckTriggered(AEVK_LBUTTON);

    }

    return false;

}

void PauseMenu::Init() {
    // 1. Set background (optional, as the level is still behind it)
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    if (pButtonMesh) return; // Prevent double initialization

    // 2. Create a generic rectangle mesh for all buttons
    // Based on your screenshot, buttons are wide rectangles
    AEGfxMeshStart();

    // Creating a 1x1 unit square to scale easily later
    // White color so we can tint it if needed
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

    pButtonMesh = AEGfxMeshEnd();
}


void PauseMenu::Update() {
    // 1. RESUME Logic
    if (IsButtonClicked(0, 100, 300, 60)) {
        next = previousState;
    }

    // 2. RESTART Logic
    if (IsButtonClicked(0, 0, 300, 60)) {
        pendingAction = GAME_STATE_TYPE::WORLD; 
        isRestartConfirm = true;
        next = GAME_STATE_TYPE::CONFIRMATION;
    }

    // 3. MAIN MENU Logic
    if (IsButtonClicked(0, -200, 300, 60)) {
        isRestartConfirm = false;
        next = GAME_STATE_TYPE::CONFIRMATION;
    }
}

void PauseMenu::Render() {
    
    // 1. Draw Title
    TextManager::DrawText("GAME PAUSED", -100.0f, 300.0f, 1.5f);

    // Helper to draw the rectangular button outline
    auto DrawBtn = [](float x, float y, const char* label) {
        AEMtx33 transform, scale, trans;
        AEMtx33Scale(&scale, 300, 60);
        AEMtx33Trans(&trans, x, y);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(pButtonMesh, AE_GFX_MDM_LINES_STRIP);

        TextManager::DrawText(label, x - 50.0f, y - 10.0f, 1.0f);
        };

        // Cast (s8*) to solve incompatibility
        // Use offset coordinates for AEGfxPrint (-1 to 1)
        //float xOffset = (x - (strlen(label) * 10.0f)) / (AEGfxGetWindowWidth() / 2.0f);
        //float yOffset = (y - 10.0f) / (AEGfxGetWindowHeight() / 2.0f);

    DrawBtn(0, 100, "RESUME");
    DrawBtn(0, 0, "RESTART");
    DrawBtn(0, -100, "AUDIO");
    DrawBtn(0, -200, "MAIN MENU");
}

// --- Confirmation Logic ---
void ConfirmationMenu::Init() {
    // empty for the confirmation popup
}


void ConfirmationMenu::Update() {
    if (IsButtonClicked(-150, -100, 150, 60)) next = GAME_STATE_TYPE::PAUSE; // NO option
    if (IsButtonClicked(150, -100, 150, 60)) { // YES option
        next = isRestartConfirm ? previousState : GAME_STATE_TYPE::MENU;
    }
}

void ConfirmationMenu::Render() {
    TextManager::DrawText("ARE YOU SURE?", -150.0f, 150.0f, 1.2f);

    auto DrawConfirmBtn = [](float x, float y, const char* label) {
        AEMtx33 transform, scale, trans;
        AEMtx33Scale(&scale, 150, 60);
        AEMtx33Trans(&trans, x, y);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(pButtonMesh, AE_GFX_MDM_LINES_STRIP);
        
        TextManager::DrawText(label, x - 20.0f, y - 10.0f);
        };

    DrawConfirmBtn(-150, -100, "NO");
    DrawConfirmBtn(150, -100, "YES");
}

void PauseMenu::Free() {
    // Unload the mesh from the GPU
    if (pButtonMesh) {
        AEGfxMeshFree(pButtonMesh);
        pButtonMesh = nullptr;
    }
}

void PauseMenu::Unload() {
    // Usually empty unless loaded textures specifically for this menu
}