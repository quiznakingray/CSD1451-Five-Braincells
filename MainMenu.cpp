#include "MainMenu.h" // Ensure this file exists in your project folder
#include <iostream>

// If AEEngine.h isn't already included via mainmenu.hpp, 
// you might need it here for the functions like AEGfxPrint
#include "AEEngine.h" 

static s8 menuFont;
static f32 startW, startH, exitW, exitH;
static const char* strStart = "START GAME";
static const char* strExit = "EXIT";

static f32 startColorR = 1.0f;
static f32 exitColorR = 1.0f;

void MainMenu_Init() {
    menuFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 72);
    AEGfxGetPrintSize(menuFont, strStart, 1.0f, &startW, &startH);
    AEGfxGetPrintSize(menuFont, strExit, 1.0f, &exitW, &exitH);
}

void MainMenu_Update() {
    s32 mX, mY;
    AEInputGetCursorPosition(&mX, &mY);

    // Coordinate conversion logic
    f32 normX = (mX / 800.0f) - 1.0f;
    f32 normY = -((mY / 450.0f) - 1.0f);

    // Start Button Logic
    if (normX > -startW / 2 && normX < startW / 2 && normY > -0.05f && normY < 0.05f) {
        startColorR = 0.0f;
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            std::cout << "Start Game Triggered!" << std::endl;
        }
    }
    else {
        startColorR = 1.0f;
    }

    // Exit Button Logic
    if (normX > -exitW / 2 && normX < exitW / 2 && normY > -0.25f && normY < -0.15f) {
        exitColorR = 0.0f;
        if (AEInputCheckTriggered(AEVK_LBUTTON)) {
            AESysExit();
        }
    }
    else {
        exitColorR = 1.0f;
    }
}

void MainMenu_Draw() {
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
    AEGfxPrint(menuFont, "ALPHA ENGINE", -0.4f, 0.5f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(menuFont, strStart, -startW / 2, 0.0f, 1.0f, startColorR, 1.0f, 1.0f, 1.0f);
    AEGfxPrint(menuFont, strExit, -exitW / 2, -0.2f, 1.0f, exitColorR, 1.0f, 1.0f, 1.0f);
}

void MainMenu_Free() {
    AEGfxDestroyFont(menuFont);
}