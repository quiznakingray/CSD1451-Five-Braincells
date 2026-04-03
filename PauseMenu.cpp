#include "TextManager.h"
#include "PauseMenu.h"
#include "GameStateManager.h"
#include "AEEngine.h"
#include "ParkourLevel.h"
#include "SpriteManager.h"
#include "TextComponent.h"
#include "CollisionManager.h"
#include "LoadingScreen.h"
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
GameObject* PauseButton(float w, float h, float x, float y,
    const char* label, std::vector<GameObject*>& vec, std::function<void()> onClick)
{
    GameObject* btn = new GameObject(w, h, x, y, 1, 0, true);
    btn->AddComponent(new Sprite())->meshColor = 0xFF757575;

    Collider* col = btn->AddComponent(new Collider());
    col->canInteract = true;
    col->OnMouseUp = onClick;

    Text* text = btn->AddComponent(new Text());
    text->inWorldSpace = false;
    text->SetText(label);

    AddGameObjectToVector(btn, vec);
    return btn;
}
void PauseMenu::Init()
{
    showConfirmation = false;
    // Background overlay
    GameObject* bg = new GameObject(AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), 0, 0, 0, 0, true);
    Sprite* bgSprite = bg->AddComponent(new Sprite());
    bgSprite->meshColor = 0x77000000;
    AddGameObjectToVector(bg, gameObjectVector);

    // Panel
    GameObject* panel = new GameObject(AEGfxGetWindowWidth() / 2.f, AEGfxGetWindowHeight() - 100.f, 0, 0, 0, 0, true);
    Sprite* panelSprite = panel->AddComponent(new Sprite());
    panelSprite->meshColor = 0xFF000000;
    AddGameObjectToVector(panel, gameObjectVector);

    PauseButton(300, 60, 0, 100, "RESUME", gameObjectVector, [] {
        GameStateManager::GetInstance().showPauseMenu = false;
        });
    PauseButton(300, 60, 0, 0, "RESTART", gameObjectVector, [] {
        isRestartConfirm = true;
        PauseMenu::GetInstance().showConfirmation = true;
        ConfirmationMenu::GetInstance().Init();
        });
    PauseButton(300, 60, 0, -100, "AUDIO", gameObjectVector, [] {
        // audio logic
        });
    PauseButton(300, 60, 0, -200, "MAIN MENU", gameObjectVector, [] {
        isRestartConfirm = false;
        PauseMenu::GetInstance().showConfirmation = true;
        ConfirmationMenu::GetInstance().Init();
        });

    InitGameObjects(gameObjectVector);
    ConfirmationMenu::GetInstance().Init();
    ConfirmationMenu::GetInstance().Hide();
}

void PauseMenu::Update() {
    // 1. RESUME Logic
    //if (IsButtonClicked(0, 100, 300, 60)) {
    //    //next = previous;
    //    //next = previous;
    //    GameStateManager::GetInstance().isGamePause = false;
    //}

    //// 2. RESTART Logic
    //if (IsButtonClicked(0, 0, 300, 60)) {
    //    //pendingAction = GAME_STATE_TYPE::WORLD; 
    //    isRestartConfirm = true;
    //    //next = GAME_STATE_TYPE::CONFIRMATION;
    //}

    //// 3. MAIN MENU Logic
    //if (IsButtonClicked(0, -200, 300, 60)) {
    //    isRestartConfirm = false;
    //    //next = GAME_STATE_TYPE::CONFIRMATION;
    //}
    if (!showConfirmation)
        UpdateGameObjects(gameObjectVector);
    else
        ConfirmationMenu::GetInstance().Update();
}

void PauseMenu::Render() {
    RenderGameObjects(gameObjectVector);
    //AEGfxPrint(TextManager::pFont, "help", -1, 0, 1, 1, 1, 1, 1);
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    //AEGfxSetTransparency(1.0f);
    //// 1. Draw Title
    TextManager::DrawText("GAME PAUSED", -100.0f, 300.0f, 1.5f);

    //// Helper to draw the rectangular button outline
    //auto DrawBtn = [](float x, float y, const char* label) {
    //    /*AEMtx33 transform, scale, trans;
    //    AEMtx33Scale(&scale, 300, 60);
    //    AEMtx33Trans(&trans, x, y);
    //    AEMtx33Concat(&transform, &trans, &scale);
    //    AEGfxSetTransform(transform.m);
    //    AEGfxMeshDraw(pButtonMesh, AE_GFX_MDM_LINES_STRIP);*/

    //    TextManager::DrawText(label, x - 50.0f, y - 10.0f, 1.0f);
    //};

    //    // Cast (s8*) to solve incompatibility
    //    // Use offset coordinates for AEGfxPrint (-1 to 1)
    //    //float xOffset = (x - (strlen(label) * 10.0f)) / (AEGfxGetWindowWidth() / 2.0f);
    //    //float yOffset = (y - 10.0f) / (AEGfxGetWindowHeight() / 2.0f);

    ////DrawBtn(0, 100, "RESUME");
    //DrawBtn(0, 0, "RESTART");
    //DrawBtn(0, -100, "AUDIO");
    //DrawBtn(0, -200, "MAIN MENU");
    if (showConfirmation)
        ConfirmationMenu::GetInstance().Render();
}

void PauseMenu::Free() {
    ConfirmationMenu::GetInstance().Free();
    FreeGameObjects(gameObjectVector);
    for (GameObject* g : gameObjectVector) {
        delete g;
    }
    gameObjectVector.clear();
    if (pButtonMesh) {
        AEGfxMeshFree(pButtonMesh);
        pButtonMesh = nullptr;
    }
}


void PauseMenu::Unload() {
    // Usually empty unless loaded textures specifically for this menu
}
// --- Confirmation Logic ---
void ConfirmationMenu::Init()
{
    if (!gameObjectVector.empty())
    {
        SetActiveGameObjects(gameObjectVector, true);
        return;
    }

    GameObject* panel = new GameObject(AEGfxGetWindowWidth() / 2.f, AEGfxGetWindowHeight() - 100.f, 0, 0, 0, 0, true);
    panel->AddComponent(new Sprite())->meshColor = 0xFF000000;
    AddGameObjectToVector(panel, gameObjectVector);

    PauseButton(200, 60, -150, -100, "NO", gameObjectVector, [] {
        PauseMenu::GetInstance().showConfirmation = false;
        ConfirmationMenu::GetInstance().Hide();
        });

    PauseButton(200, 60, 150, -100, "YES", gameObjectVector, [] {
        PauseMenu::GetInstance().showConfirmation = false;
        GameStateManager::GetInstance().showPauseMenu = false;

        if (isRestartConfirm)
        {
            // Restart: reload the current level through the loading screen
            LoadingScreen::targetState = previous;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        }
        else
        {
            // Main menu: go through loading screen
            LoadingScreen::targetState = GAME_STATE_TYPE::MENU;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        }
        });

    InitGameObjects(gameObjectVector);
}

void ConfirmationMenu::Hide()
{
    SetActiveGameObjects(gameObjectVector, false);
    PauseMenu::GetInstance().showConfirmation = false;
}

void ConfirmationMenu::Update() {
    //if (IsButtonClicked(-150, -100, 150, 60)) next = GAME_STATE_TYPE::PAUSE; // NO option
    //if (IsButtonClicked(150, -100, 150, 60)) { // YES option
    //    next = isRestartConfirm ? previous : GAME_STATE_TYPE::MENU;
    //}
    UpdateGameObjects(gameObjectVector);
}

void ConfirmationMenu::Render() {

   /* auto DrawConfirmBtn = [](float x, float y, const char* label) {
        AEMtx33 transform, scale, trans;
        AEMtx33Scale(&scale, 150, 60);
        AEMtx33Trans(&trans, x, y);
        AEMtx33Concat(&transform, &trans, &scale);
        AEGfxSetTransform(transform.m);
        AEGfxMeshDraw(pButtonMesh, AE_GFX_MDM_LINES_STRIP);
        
        TextManager::DrawText(label, x - 20.0f, y - 10.0f);
        };

    DrawConfirmBtn(-150, -100, "NO");
    DrawConfirmBtn(150, -100, "YES");*/
    RenderGameObjects(gameObjectVector);
    TextManager::DrawText("ARE YOU SURE?", -150.0f, 150.0f, 1.2f);
}

void ConfirmationMenu::Free()
{
    FreeGameObjects(gameObjectVector);
    for (GameObject* g : gameObjectVector) {
        delete g;
    }
    gameObjectVector.clear();
}