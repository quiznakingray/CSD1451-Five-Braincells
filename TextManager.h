#ifndef TEXT_MANAGER_H
#define TEXT_MANAGER_H

#include "AEEngine.h"

struct TextManager {
    static s8 pFont; // The ID for the loaded font

    static void Init();
    // Helper to draw text using pixel coordinates
    static void DrawText(const char* text, float x, float y, float scale = 1.0f);
};

#endif
