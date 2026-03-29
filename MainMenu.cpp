#include "MainMenu.h" // Ensure this file exists in your project folder
#include "GameStateManager.h"
#include "AudioMenu.h"
#include <vector>
#include <string> // For strcmp
#include <cstdio> // For sprintf

// If AEEngine.h isn't already included via mainmenu.hpp, 
// you might need it here for the functions like AEGfxPrint
#include "AEEngine.h" 

extern int gGameRunning; 
static s8 menuFont;
static AEGfxVertexList* pRectMesh;

static bool isAudioMenuOpen = false;
static AudioMenu audioMenu;
static AEGfxTexture* pBackgroundTex; // Background variable
static int highScore = 1500;         // Example high score

struct Button {
    const char* text;
    f32 yPos;
    f32 w, h;
    bool isHovered;
};
//static f32 startW, startH, exitW, exitH;
//static const char* strStart = "START GAME";
//static const char* strExit = "EXIT";

//static f32 startColorR = 1.0f;
//static f32 exitColorR = 1.0f;

// Define the buttons
static std::vector<Button> buttons;


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

    audioMenu.Init(); // initialize audio panel

    // Play main menu bgm
    //AudioManager::GetInstance().GetInstance().PlayMusic("menu_bgm");
}

void MainMenu_Update() {
    s32 mX, mY;
    AEInputGetCursorPosition(&mX, &mY);

    // Checks if audio panel is open
    if (isAudioMenuOpen)
    {
        audioMenu.Update();

        // Escape button closes audio panel
        if (AEInputCheckTriggered(AEVK_ESCAPE))
        {
            isAudioMenuOpen = false;
        }
        return; // stop main menu interaction when audio panel is open
    }

    // Get current dimensions dynamically
    f32 halfWidth = AEGfxGetWindowWidth() / 2.0f;
    f32 halfHeight = AEGfxGetWindowHeight() / 2.0f;

    // Convert screen pixels to normalized coordinates (-1 to 1)
    f32 normX = (static_cast<f32>(mX) / halfWidth) - 1.0f;
    f32 normY = -((static_cast<f32>(mY) / halfHeight) - 1.0f);

    for (auto& btn : buttons) {
        // AABB Collision check
        btn.isHovered = (normX > -btn.w / 2.0f && normX < btn.w / 2.0f &&
            normY > btn.yPos - 0.05f && normY < btn.yPos + 0.05f);

        if (btn.isHovered && AEInputCheckTriggered(AEVK_LBUTTON)) {
            if (strcmp(btn.text, "NEW GAME") == 0) {
                SaveManager::GetInstance().ResetSave();  // clears everything including preserveOnLoad
                next = GAME_STATE_TYPE::LEVEL1;
            }
            if (strcmp(btn.text, "LOAD GAME") == 0) {
                if (SaveManager::GetInstance().HasSaveData()) {
                    SaveManager::GetInstance().LoadPlayerData();
                    SaveManager::GetInstance().LoadMapData();
                    SaveManager::GetInstance().LoadEnemyData();
                    SaveManager::GetInstance().toContinue = true;
                    next = SaveManager::GetInstance().mapSaveData.savedLevel;
                }
            }

            if (strcmp(btn.text, "INSTRUCTIONS") == 0) next = GAME_STATE_TYPE::INSTRUCTIONS;
            if (strcmp(btn.text, "EXIT") == 0) gGameRunning = 0;
            // Checks if settings button is clicked
            if (strcmp(btn.text, "SETTING") == 0)
            {
                isAudioMenuOpen = true; // open audio panel
            }
        }
    }
}

void MainMenu_Draw() {
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    // Draw Title: Separated at the top
    // AEGfxPrint takes 9 arguments: font, text, x, y, scale, r, g, b, a
    AEGfxPrint(menuFont, "DUNGEON & PUZZLE", -0.4f, 0.6f, 1.1f, 1.0f, 0.8f, 0.0f, 1.0f);

    // Checks if audio panel is open
    if (isAudioMenuOpen)
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
    AEGfxPrint(menuFont, scoreBuffer, -0.35f, 0.2f, 0.8f, 0.5f, 1.0f, 1.0f, 1.0f);

    // 3. Draw Buttons
    for (const auto& btn : buttons) {
        f32 r = btn.isHovered ? 1.0f : 0.7f;
        f32 g = btn.isHovered ? 1.0f : 0.7f;
        f32 b = btn.isHovered ? 0.0f : 0.7f;

        // Using your original centering logic (-btn.w / 2.0f)
        AEGfxPrint(menuFont, btn.text, -btn.w / 2.0f, btn.yPos, 1.0f, r, g, b, 1.0f);
    }
}

void MainMenu_Free() {
    AEGfxDestroyFont(menuFont);
    AEGfxMeshFree(pRectMesh);

    audioMenu.Free(); // free audio panel
}