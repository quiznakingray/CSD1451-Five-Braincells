#include "MainMenu.h"
#include "GameStateManager.h"
#include "AudioManager.h"
#include "AudioMenu.h"
#include "LoadingScreen.h"
#include "SaveManager.h"
#include "AEEngine.h"

extern int gGameRunning;

static std::vector<GameObject*> menuObjects;
static std::vector<GameObject*> warningObjects;
static bool showOverwriteWarning = false;

static GameObject* MakeButton(float w, float h, float x, float y,
    const char* label, std::vector<GameObject*>& vec, std::function<void()> onClick,
    bool disabled = false)
{
    GameObject* btn = new GameObject(w, h, x, y, 1, 0, true);
    Sprite* idle = btn->AddComponent(new Sprite());
    idle->textureFileName = "Assets/TEMP_Sprites/button_idle.png";

    Collider* col = btn->AddComponent(new Collider());
    col->canInteract = !disabled;

    Sprite* sprite = idle; // capture for hover
    col->OnMouseEnter = [sprite]() {
        sprite->textureFileName = "Assets/TEMP_Sprites/button_hover.png";
        };
    col->OnMouseExit = [sprite]() {
        sprite->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
        };
    col->OnMouseUp = onClick;

    Text* text = btn->AddComponent(new Text());
    text->inWorldSpace = false;
    text->SetText(label);

    if (disabled)
    {
        // grey out
        text->SetColor({1.f, 1.f, 1.f, 0.5f});
        sprite->multiplyColor = {0.75f, 0.75f, 0.75f, 1.0f};
    }
    else {

        text->SetColor({1.f, 1.f, 1.f, 1.f});
    }

    AddGameObjectToVector(btn, vec);
    return btn;
}

void MainMenu_Init()
{
    if (!menuObjects.empty()) MainMenu_Free();

    float winW = (float)AEGfxGetWindowWidth();
    float winH = (float)AEGfxGetWindowHeight();
    float cx = 0.f;
    float btnW = 400.f, btnH = 60.f;
    float startY = 100.f;
    float gap = 100.f;

    bool hasSave = SaveManager::GetInstance().HasSaveData();

    // Title
    //GameObject* title = new GameObject(600.f, 80.f, cx, winH / 2.f - 80.f, 1, 0, true);
    //title->AddComponent(new Sprite())->meshColor = 0x00000000;
    //Text* titleText = title->AddComponent(new Text());
    //titleText->inWorldSpace = false;
    //titleText->SetText("Just Two Guys");
    //titleText->size = 2.f;
    //AddGameObjectToVector(title, menuObjects);

	GameObject* bg = new GameObject(winW, winH, 0, 0, 0, 0, true);
	Sprite* bgSprite = bg->AddComponent(new Sprite());
    bgSprite->textureFileName = "Assets/Environment/bg_grasslands.png";
	AddGameObjectToVector(bg, menuObjects);
    // NEW GAME
    MakeButton(btnW, btnH, cx, startY, "NEW GAME", menuObjects, []() {
        if (SaveManager::GetInstance().HasSaveData())
            showOverwriteWarning = true;
        else {
            SaveManager::GetInstance().ResetSave();
            LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        }
        });

    // LOAD GAME
    MakeButton(btnW, btnH, cx, startY - gap, "LOAD GAME", menuObjects, []() {
        if (SaveManager::GetInstance().HasSaveData()) {
            SaveManager::GetInstance().LoadPlayerData();
            SaveManager::GetInstance().LoadMapData();
            SaveManager::GetInstance().LoadEnemyData();
            SaveManager::GetInstance().toContinue = true;
            LoadingScreen::targetState = SaveManager::GetInstance().mapSaveData.savedLevel;
            GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        }
        }, !hasSave);

    // INSTRUCTIONS
    MakeButton(btnW, btnH, cx, startY - gap * 2, "INSTRUCTIONS", menuObjects, []() {
        next = GAME_STATE_TYPE::CONTROLS;
        });

    // SETTINGS
    MakeButton(btnW, btnH, cx, startY - gap * 3, "SETTINGS", menuObjects, []() {
        if (!AudioMenu::GetInstance().IsOpen())
            AudioMenu::GetInstance().Toggle();
        });

    // CREDITS
    MakeButton(btnW, btnH, cx, startY - gap * 4, "CREDITS", menuObjects, []() {
        next = GAME_STATE_TYPE::CREDITS;
        });

    // EXIT
    MakeButton(btnW, btnH, cx, startY - gap * 5, "EXIT", menuObjects, []() {
        gGameRunning = 0;
        });

    InitGameObjects(menuObjects);

    // --- WARNING PANEL ---
    // Dark backdrop
    GameObject* warnBG = new GameObject(1100.f, 500.f, cx, 0.f, 0, 0, true);
    warnBG->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
    AddGameObjectToVector(warnBG, warningObjects);

    // Warning text
    GameObject* warnTitle = new GameObject(460.f, 60.f, cx, 120.f, 0, 0, true);
    //warnTitle->AddComponent(new Sprite())->meshColor = 0x00000000;
    Text* warnTitleText = warnTitle->AddComponent(new Text());
    warnTitleText->inWorldSpace = false;
    warnTitleText->SetText("SAVE DATA EXISTS!");
    AddGameObjectToVector(warnTitle, warningObjects);

    GameObject* warnMsg = new GameObject(460.f, 80.f, cx, 20.f, 0, 0, true);
    //warnMsg->AddComponent(new Sprite())->meshColor = 0x00000000;
    Text* warnMsgText = warnMsg->AddComponent(new Text());
    warnMsgText->inWorldSpace = false;
    warnMsgText->SetText("Starting a new game will erase your existing progress.");
    AddGameObjectToVector(warnMsg, warningObjects);

    // YES OVERWRITE
    MakeButton(300.f, 55.f, cx - 200.f, -80.f, "YES, OVERWRITE", warningObjects, []() {
        SaveManager::GetInstance().ResetSave();
        LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;
        GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
        showOverwriteWarning = false;
        });

    // CANCEL
    MakeButton(300.f, 55.f, cx + 200.f, -80.f, "CANCEL", warningObjects, []() {
        showOverwriteWarning = false;
        });

    InitGameObjects(warningObjects);

    AudioManager::GetInstance().PlayMusic("mainMenu");
}

void MainMenu_Update()
{
    // toggle warning panel objects
    for (GameObject* go : warningObjects)
        go->isActive = showOverwriteWarning;

    // block main menu interaction when warning is open
    for (GameObject* go : menuObjects)
    {
        Collider* col = go->GetComponent<Collider>();
        if (col) col->canInteract = !showOverwriteWarning;
    }

    if (AEInputCheckTriggered(AEVK_ESCAPE) && showOverwriteWarning)
        showOverwriteWarning = false;

    if (!AudioMenu::GetInstance().IsOpen() )UpdateGameObjects(menuObjects);
    if (showOverwriteWarning)
        UpdateGameObjects(warningObjects);
}

void MainMenu_Draw()
{
    AEGfxSetBackgroundColor(0.f, 0.f, 0.f);
    RenderGameObjects(menuObjects);
    if (showOverwriteWarning)
        RenderGameObjects(warningObjects);
}

void MainMenu_Free()
{
    FreeGameObjects(menuObjects);
    for (GameObject* go : menuObjects) delete go;
    menuObjects.clear();

    FreeGameObjects(warningObjects);
    for (GameObject* go : warningObjects) delete go;
    warningObjects.clear();

    showOverwriteWarning = false;
}