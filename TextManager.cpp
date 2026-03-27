#include "TextManager.h"
#include "AEEngine.h"

#include <iostream>

s8 TextManager::pFont = 0;

void TextManager::Init() {
    // Only load if it hasn't been loaded yet
    if (TextManager::pFont <= 0) {
        // Modern AE takes const char* - no cast needed
        TextManager::pFont = AEGfxCreateFont("Assets/Fonts/Arial.ttf", 32);
    }
    if (TextManager::pFont <= 0) {
        std::cout << "Font failed to load! Check path: Assets/Fonts/Arial.ttf" << std::endl;
    }
}

void TextManager::DrawText(const char* text, float x, float y, float scale) {
    if (TextManager::pFont <= 0) return;

    // Alpha Engine's AEGfxPrint uses Normalized Device Coordinates (NDC)
    // Range is -1.0 to 1.0. 
    // We convert your pixel inputs (e.g., 150, 100) to this range:
    float nX = x / (AEGfxGetWindowWidth() / 2.0f);
    float nY = y / (AEGfxGetWindowHeight() / 2.0f);

    // AEGfxPrint parameters:
    // FontID, String(s8*), X, Y, Scale, Red, Green, Blue, Alpha
    AEGfxPrint(TextManager::pFont, text, nX, nY, scale, 1.0f, 1.0f, 1.0f, 1.0f);
}