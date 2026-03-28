#include "Credits.h"
#include "GameStateManager.h"
#include "AEEngine.h"
#include <vector>

static s8 creditsFont;
static float slideTimer = 0.0f;
static int currentSlide = 0;
const float SLIDE_DURATION = 3.0f; // Seconds per slide

// Data for the slideshow
struct CreditSlide {
    const char* category;
    std::vector<const char*> names;
};

static std::vector<CreditSlide> slides;

void Credits_Init() {
    creditsFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 40);
    slideTimer = 0.0f;
    currentSlide = 0;

    // Based on requirements from image_46ba2a.png
    slides = {
        {"Team Members", {"Rachel", "Nadyah", "Xin Yi", "Qi Jun", "Zi Jun"}}, // Add your "Five - Braincells" team here
        {"Instructors", {"Soroor, Gerald, Tommy"}},
        {"President", {"Claude Comair"}},
        {"Special Thanks", {"Former Members", "Friends & Family"}}
    };
}

void Credits_Update() {
    float dt = (float)AEFrameRateControllerGetFrameTime();
    slideTimer += dt;

    // Advance slide or return to menu if finished
    if (slideTimer >= SLIDE_DURATION) {
        slideTimer = 0.0f;
        currentSlide++;

        if (currentSlide >= slides.size()) {
            next = GAME_STATE_TYPE::MENU;
        }
    }

    // Allow user to skip or go back manually
    if (AEInputCheckTriggered(AEVK_ESCAPE) || AEInputCheckTriggered(AEVK_LBUTTON)) {
        next = GAME_STATE_TYPE::MENU;
    }
}

void Credits_Draw() {
    AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

    // REQUIREMENT: Official DigiPen copyright MUST be at the top
    AEGfxPrint(creditsFont, "Copyright (C) 2026 DigiPen Institute of Technology.", -0.8f, 0.8f, 0.8f, 1.0f, 1.0f, 1.0f, 1.0f);

    if (currentSlide < slides.size()) {
        // Draw Category
        AEGfxPrint(creditsFont, slides[currentSlide].category, -0.2f, 0.3f, 1.2f, 1.0f, 0.8f, 0.0f, 1.0f);

        // Draw Names
        float yOffset = 0.1f;
        for (const char* name : slides[currentSlide].names) {
            AEGfxPrint(creditsFont, name, -0.15f, yOffset, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
            yOffset -= 0.15f;
        }
    }
}

void Credits_Free() {
    AEGfxDestroyFont(creditsFont);
}