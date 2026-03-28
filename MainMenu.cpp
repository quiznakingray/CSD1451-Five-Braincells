#include "MainMenu.h" // Ensure this file exists in your project folder
#include "GameStateManager.h"
#include "AudioMenu.h"
#include <vector>
#include <string> // For strcmp
//#include <iostream>

// If AEEngine.h isn't already included via mainmenu.hpp, 
// you might need it here for the functions like AEGfxPrint
#include "AEEngine.h" 

extern int gGameRunning; 
static s8 menuFont;
static AEGfxVertexList* pRectMesh;

static bool isAudioMenuOpen = false;
static AudioMenu audioMenu;

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
        {"PLAY", 0.0f, 0, 0, false},
        {"CONTINUE", -0.15f, 0, 0, false},
        {"TUTORIAL", -0.30f, 0, 0, false},
        {"SETTINGS", -0.45f, 0, 0, false},
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
            if (strcmp(btn.text, "PLAY") == 0) {
                SaveManager::GetInstance().ResetSave();  // clears everything including preserveOnLoad
                next = GAME_STATE_TYPE::LEVEL1;
            }
            if (strcmp(btn.text, "CONTINUE") == 0) {
                if (SaveManager::GetInstance().HasSaveData()) {
                    SaveManager::GetInstance().LoadPlayerData();
                    SaveManager::GetInstance().LoadMapData();
                    SaveManager::GetInstance().LoadEnemyData();
                    SaveManager::GetInstance().toContinue = true;
                    next = SaveManager::GetInstance().mapSaveData.savedLevel;
                }
            }
            if (strcmp(btn.text, "EXIT") == 0) gGameRunning = 0;

            // Checks if settings button is clicked
            if (strcmp(btn.text, "SETTINGS") == 0)
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
    AEGfxPrint(menuFont, "DUNGEON AND PUZZLE", -0.55f, 0.6f, 1.5f, 1.0f, 0.8f, 0.0f, 1.0f);

    // Checks if audio panel is open
    if (isAudioMenuOpen)
    {
        audioMenu.Render(); // renders audio panel
        return;
    }

    // Draw Buttons: Centralized and equally separated
    for (const auto& btn : buttons) {
        f32 r = btn.isHovered ? 1.0f : 0.7f;
        f32 g = btn.isHovered ? 1.0f : 0.7f;
        f32 b = btn.isHovered ? 0.0f : 0.7f;

        // 2. Transform the mesh to fit the button dimensions
        //AEMtx33 transform;
        //AEMtx33Scale(&transform, btn.w + 0.1f, btn.h + 0.08f);
        //AEMtx33Trans(&transform, 0.0f, btn.yPos + 0.02f);
        //AEGfxSetTransform(transform.m);

        //// 3. Render using color mode (no texture)
        //AEGfxSetRenderMode(AE_GFX_RM_COLOR);
        //AEGfxSetColorToMultiply(r, g, b, 1.0f);
        //AEGfxMeshDraw(pRectMesh, AE_GFX_MDM_TRIANGLES);

        //// Reset transform for text
        //AEMtx33 identity;
        //AEMtx33Identity(&identity);
        //AEGfxSetTransform(identity.m);

        AEGfxPrint(menuFont, btn.text, -btn.w / 2.0f, btn.yPos, 1.0f, r, g, b, 1.0f);
    }
}

void MainMenu_Free() {
    AEGfxDestroyFont(menuFont);
    AEGfxMeshFree(pRectMesh);

    audioMenu.Free(); // free audio panel
}