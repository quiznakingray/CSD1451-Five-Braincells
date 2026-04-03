#include "MainMenu.h" // Ensure this file exists in your project folder
#include "GameStateManager.h"
#include "AudioManager.h"
#include "AudioMenu.h"
#include "LoadingScreen.h"
#include <vector>
#include <string> // For strcmp
#include <cstdio> // For sprintf

// If AEEngine.h isn't already included via mainmenu.hpp, 
// you might need it here for the functions like AEGfxPrint
#include "AEEngine.h" 

extern int gGameRunning; 
static s8 menuFont;
static AEGfxVertexList* pRectMesh;

static AudioMenu audioMenu;
static AEGfxTexture* pBackgroundTex; // Background variable
static int highScore = 1500;         // Example high score

struct Button {
    const char* text;
    f32 yPos;
    f32 w, h;
    bool isHovered;
    bool disabled = false;
};
//static f32 startW, startH, exitW, exitH;
//static const char* strStart = "START GAME";
//static const char* strExit = "EXIT";

//static f32 startColorR = 1.0f;
//static f32 exitColorR = 1.0f;

// Define the buttons
static std::vector<Button> buttons;
static std::vector<Button> warnButtons;
static bool showOverwriteWarning = false;

static void DrawRect(f32 cx, f32 cy, f32 w, f32 h)
{
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    AEMtx33 scale, trans, transform;
    AEMtx33Scale(&scale, w, h);
    AEMtx33Trans(&trans, cx, cy);
    AEMtx33Concat(&transform, &trans, &scale);
    AEGfxSetTransform(transform.m);

    AEGfxMeshDraw(pRectMesh, AE_GFX_MDM_TRIANGLES);
}

void MainMenu_Init() {
    // Load font at 60px height
    menuFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 60);


    // Initialize button list
    buttons = {
        {"NEW GAME", 0.0f, 0, 0, false},
        {"LOAD GAME", -0.15f, 0, 0, false},
        {"INSTRUCTIONS", -0.30f, 0, 0, false},
        {"SETTING", -0.45f, 0, 0, false},
        {"CREDITS", -0.60f, 0, 0, false},
        {"EXIT", -0.75f, 0, 0, false}
    };

    // overwrite warning buttons
    warnButtons = {
       {"YES, OVERWRITE", -0.5f, 0, 0, false},
       {"CANCEL",         -0.3f, 0, 0, false}
    };

    // 1. Create a 1x1 white square mesh centered at origin
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    AEGfxTriAdd(0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    pRectMesh = AEGfxMeshEnd();

    // Pre-calculate sizes using the correct 5 arguments
    for (auto& btn : buttons) {
        AEGfxGetPrintSize(menuFont, btn.text, 1.0f, &btn.w, &btn.h);
    }
    for (auto& btn : warnButtons) {
        AEGfxGetPrintSize(menuFont, btn.text, 1.0f, &btn.w, &btn.h);
    }

    // Dim LOAD GAME if no save exists
    for (auto& btn : buttons) {
        if (strcmp(btn.text, "LOAD GAME") == 0)
            btn.disabled = !SaveManager::GetInstance().HasSaveData();
    }
    audioMenu.Init(); // initialize audio panel

    // Play main menu bgm
    AudioManager::GetInstance().PlayMusic("mainMenu");
}

void MainMenu_Update() {
    s32 mX, mY;
    AEInputGetCursorPosition(&mX, &mY);

    // Checks if audio panel is open
    if (audioMenu.IsOpen())
    {
        audioMenu.Update();

        // Escape button closes audio panel
        if (AEInputCheckTriggered(AEVK_ESCAPE) && audioMenu.IsOpen())
        {
            audioMenu.Toggle();
        }
        return; // stop main menu interaction when audio panel is open
    }

    // Get current dimensions dynamically
    f32 halfWidth = AEGfxGetWindowWidth() / 2.0f;
    f32 halfHeight = AEGfxGetWindowHeight() / 2.0f;

    // Convert screen pixels to normalized coordinates (-1 to 1)
    f32 normX = (static_cast<f32>(mX) / halfWidth) - 1.0f;
    f32 normY = -((static_cast<f32>(mY) / halfHeight) - 1.0f);

    // Warning panel takes priority over main buttons
    if (showOverwriteWarning) {
        if (AEInputCheckTriggered(AEVK_ESCAPE)) {
            showOverwriteWarning = false;
            return;
        }

        for (auto& btn : warnButtons) {
            // Identical AABB hover check as main buttons
            btn.isHovered = (normX > -btn.w / 2.0f && normX < btn.w / 2.0f &&
                normY > btn.yPos - 0.05f && normY < btn.yPos + 0.05f);

            if (btn.isHovered && AEInputCheckTriggered(AEVK_LBUTTON)) {
                if (strcmp(btn.text, "YES, OVERWRITE") == 0) {
                    SaveManager::GetInstance().ResetSave();
                    LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;
                    GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
                    showOverwriteWarning = false;
                }
                if (strcmp(btn.text, "CANCEL") == 0) {
                    showOverwriteWarning = false;
                }
            }
        }
        return; // block main menu while warning is open
    }

    for (auto& btn : buttons) {
        // AABB Collision check
        btn.isHovered = !btn.disabled && (normX > -btn.w / 2.0f && normX < btn.w / 2.0f &&
            normY > btn.yPos - 0.05f && normY < btn.yPos + 0.05f);

        if (btn.isHovered && AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (strcmp(btn.text, "NEW GAME") == 0) {
                if (SaveManager::GetInstance().HasSaveData())
                {
                    showOverwriteWarning = true;
                }
                else {
                    SaveManager::GetInstance().ResetSave();
                    LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;
                    GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
                }
            }
            if (strcmp(btn.text, "LOAD GAME") == 0) {
                if (SaveManager::GetInstance().HasSaveData()) {
                    SaveManager::GetInstance().LoadPlayerData();
                    SaveManager::GetInstance().LoadMapData();
                    SaveManager::GetInstance().LoadEnemyData();
                    SaveManager::GetInstance().toContinue = true;
                    LoadingScreen::targetState = SaveManager::GetInstance().mapSaveData.savedLevel;
                    GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
                }
            }

            if (strcmp(btn.text, "INSTRUCTIONS") == 0) next = GAME_STATE_TYPE::INSTRUCTIONS;
            if (strcmp(btn.text, "CREDITS") == 0) next = GAME_STATE_TYPE::CREDITS;
            if (strcmp(btn.text, "EXIT") == 0) gGameRunning = 0;
            // Checks if settings button is clicked
            if (strcmp(btn.text, "SETTING") == 0 && !audioMenu.IsOpen())
            {
                audioMenu.Toggle(); // open audio panel
            }
        }
    }
}

void MainMenu_Draw() {
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    // Draw Title: Separated at the top
    // AEGfxPrint takes 9 arguments: font, text, x, y, scale, r, g, b, a
    f32 titleA = showOverwriteWarning ? 0.15f : 1.0f;
    AEGfxPrint(menuFont, "DUNGEON & PUZZLE", -0.4f, 0.6f, 1.1f, 1.0f * titleA, 0.8f * titleA, 0.0f, titleA);

    // Checks if audio panel is open
    if (audioMenu.IsOpen())
    {
        audioMenu.Render(); // renders audio panel
        return;
    }

    // Draw Buttons: Centralized and equally separated
    // 2. Draw Highest Score (Added this part)
    char scoreBuffer[32];
    // This converts the integer highScore into a "HIGHEST SCORE: 1500" string
    sprintf_s(scoreBuffer, "HIGHEST SCORE: %d", highScore);

    // Positioned at y = 0.2f (above NEW GAME) with a white/cyan color
    f32 alpha = showOverwriteWarning ? 0.15f : 1.0f;
    AEGfxPrint(menuFont, scoreBuffer, -0.35f, 0.2f, 0.8f, 0.5f * alpha, 1.0f * alpha, 1.0f * alpha, alpha);

    // Main buttons — dimmed when warning panel is open
    for (const auto& btn : buttons) {
        f32 a = (showOverwriteWarning || btn.disabled) ? 0.15f : 1.0f;
        f32 r = btn.isHovered ? 1.0f : 0.7f;
        f32 g = btn.isHovered ? 1.0f : 0.7f;
        f32 b = btn.isHovered ? 0.0f : 0.7f;
        AEGfxPrint(menuFont, btn.text, -btn.w / 2.0f, btn.yPos, 1.0f, r * a, g * a, b * a, 1.0f);
    }

    // Warning panel
    if (showOverwriteWarning) {
        // Warning text
        AEGfxPrint(menuFont, "SAVE DATA EXISTS!", -0.3f, 0.25f, 0.85f, 1.0f, 0.3f, 0.3f, 1.0f);
        AEGfxPrint(menuFont, "Starting a new game", -0.275f, 0.12f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(menuFont, "will erase your", -0.2f, 0.02f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxPrint(menuFont, "existing progress.", -0.25f, -0.08f, 0.7f, 1.0f, 1.0f, 1.0f, 1.0f);

        // Warning buttons — identical render logic to main buttons
        for (const auto& btn : warnButtons) {
            bool isOverwrite = (strcmp(btn.text, "YES, OVERWRITE") == 0);

            // Overwrite = red tones, Cancel = green tones
            f32 r = isOverwrite ? (btn.isHovered ? 1.0f : 0.8f) : (btn.isHovered ? 0.3f : 0.2f);
            f32 g = isOverwrite ? (btn.isHovered ? 0.3f : 0.2f) : (btn.isHovered ? 1.0f : 0.8f);
            f32 b = isOverwrite ? (btn.isHovered ? 0.3f : 0.2f) : (btn.isHovered ? 0.3f : 0.2f);

            AEGfxPrint(menuFont, btn.text, -btn.w / 2.0f, btn.yPos, 1.0f, r, g, b, 1.0f);
        }
    }
}

void MainMenu_Free() {
    AEGfxDestroyFont(menuFont);
    AEGfxMeshFree(pRectMesh);

    audioMenu.Free(); // free audio panel
    buttons.clear();
	warnButtons.clear();
}