//---------------------------------------------------------
// author:    Ang Zi Jun & Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "TextManager.h"
#include "PauseMenu.h"
#include "GameStateManager.h"
#include "AEEngine.h"
#include "ParkourLevel.h"
#include "SpriteManager.h"
#include "TextComponent.h"
#include "CollisionManager.h"
#include "LoadingScreen.h"
#include "AudioMenu.h"
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
    const char* label, std::vector<GameObject*>& vec, std::function<void()> onClick, Color multiply = {1.f, 1.f, 1.f, 1.f})
{
    GameObject* btn = new GameObject(w, h, x, y, 1, 0, true);

	Sprite* sprite = btn->AddComponent(new Sprite());
	sprite->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
    sprite->multiplyColor = multiply;
    Collider* col = btn->AddComponent(new Collider());
    col->canInteract = true;
    col->OnMouseUp = onClick;
	col->OnMouseEnter = [btn]() { 
		btn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_hover.png";
    };
	col->OnMouseExit = [btn]() { 
		btn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
    };

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
    GameObject* bg = new GameObject(static_cast<f32>(AEGfxGetWindowWidth()), static_cast<f32>(AEGfxGetWindowHeight()), 0, 0, 0, 0, true);
    Sprite* bgSprite = bg->AddComponent(new Sprite());
    bgSprite->meshColor = 0x77000000;
	
    AddGameObjectToVector(bg, pauseGameObjectVector);

    // Panel
    GameObject* panel = new GameObject(AEGfxGetWindowWidth() / 2.f, AEGfxGetWindowHeight() - 100.f, 0, 0, 0, 0, true);
    Sprite* panelSprite = panel->AddComponent(new Sprite());
    //panelSprite->meshColor = 0xFF000000;
    panelSprite->textureFileName = "Assets/TEMP_Sprites/audio_panel.png";
    AddGameObjectToVector(panel, pauseGameObjectVector);

    PauseButton(300, 60, 0, 100, "RESUME", pauseGameObjectVector, [] {
        GameStateManager::GetInstance().showPauseMenu = false;
        GameStateManager::GetInstance().gamePaused = false;
        });
    PauseButton(300, 60, 0, 0, "RESTART", pauseGameObjectVector, [] {
        isRestartConfirm = true;
        PauseMenu::GetInstance().showConfirmation = true;
        ConfirmationMenu::GetInstance().Init();
        });
    PauseButton(300, 60, 0, -100, "AUDIO", pauseGameObjectVector, [] {
            if (!AudioMenu::GetInstance().IsOpen())
            {
				AudioMenu::GetInstance().Toggle();

            }
        });
    PauseButton(300, 60, 0, -200, "MAIN MENU", pauseGameObjectVector, [] {
        isRestartConfirm = false;
        PauseMenu::GetInstance().showConfirmation = true;
        ConfirmationMenu::GetInstance().Init();
        });

    InitGameObjects(pauseGameObjectVector);
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
    if (!showConfirmation && !AudioMenu::GetInstance().IsOpen())
        UpdateGameObjects(pauseGameObjectVector);
    
	if (showConfirmation)
        ConfirmationMenu::GetInstance().Update();
}

void PauseMenu::Render() {
    RenderGameObjects(pauseGameObjectVector);
    //AEGfxPrint(TextManager::pFont, "help", -1, 0, 1, 1, 1, 1, 1);
    //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    //AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    //AEGfxSetTransparency(1.0f);
    //// 1. Draw Title
    TextManager::DrawText("GAME PAUSED", -155.0f, 320.0f, 1.5f);

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
    FreeGameObjects(pauseGameObjectVector);
    for (GameObject* g : pauseGameObjectVector) {
        delete g;
    }
    pauseGameObjectVector.clear();
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
    if (!confirmationGameObjectVector.empty())
    {
        SetActiveGameObjects(confirmationGameObjectVector, true);
        return;
    }
    GameObject* bg = new GameObject(static_cast<f32>(AEGfxGetWindowWidth()), static_cast<f32>(AEGfxGetWindowHeight()), 0, 0, 0, 0, true);
    Sprite* bgSprite = bg->AddComponent(new Sprite());
    bgSprite->meshColor = 0x77000000;

    AddGameObjectToVector(bg, confirmationGameObjectVector);

    GameObject* panel = new GameObject(AEGfxGetWindowWidth() / 3.f, AEGfxGetWindowHeight() - 500.f, 0, 0, 0, 0, true);
    panel->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/audio_panel.png";
    AddGameObjectToVector(panel, confirmationGameObjectVector);

    PauseButton(200, 60, -150, 0, "NO", confirmationGameObjectVector, [] {
        PauseMenu::GetInstance().showConfirmation = false;
        ConfirmationMenu::GetInstance().Hide();
        },
        { 1.0f, 1.0f, 1.2f, 1.f });

    PauseButton(200, 60, 150, 0, "YES", confirmationGameObjectVector, [] {
        PauseMenu::GetInstance().showConfirmation = false;
        GameStateManager::GetInstance().showPauseMenu = false;
        GameStateManager::GetInstance().gamePaused = false;


        if (isRestartConfirm)
        {
            // Restart: reload the current level through the loading screen
            LoadingScreen::targetState = current;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        }
        else
        {
            // Main menu: go through loading screen
            LoadingScreen::targetState = GAME_STATE_TYPE::MENU;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        }
        }
    );

    InitGameObjects(confirmationGameObjectVector);
}

void ConfirmationMenu::Hide()
{
    SetActiveGameObjects(confirmationGameObjectVector, false);
    PauseMenu::GetInstance().showConfirmation = false;
}

void ConfirmationMenu::Update() {
    //if (IsButtonClicked(-150, -100, 150, 60)) next = GAME_STATE_TYPE::PAUSE; // NO option
    //if (IsButtonClicked(150, -100, 150, 60)) { // YES option
    //    next = isRestartConfirm ? previous : GAME_STATE_TYPE::MENU;
    //}
    UpdateGameObjects(confirmationGameObjectVector);
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
    RenderGameObjects(confirmationGameObjectVector);
    TextManager::DrawText("ARE YOU SURE?", -150.0f, 150.0f, 1.2f);
}

void ConfirmationMenu::Free()
{
    FreeGameObjects(confirmationGameObjectVector);
    for (GameObject* g : confirmationGameObjectVector) {
        delete g;
    }
    confirmationGameObjectVector.clear();
}