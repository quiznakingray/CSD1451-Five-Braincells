#ifndef FADE_MANAGER_H
#define FADE_MANAGER_H

#include "SingletonTemplate.h"
#include "AEEngine.h"
#include "GameStateManager.h"

struct FadeManager : Singleton<FadeManager>
{

    void BeginFadeOut(GAME_STATE_TYPE target, int frames = 30);

    void BeginFadeIn(int frames = 30);

    void Update();

    void Render();

    bool IsFading() const { return fadeOutActive || fadeInActive; }

    ~FadeManager();

private:
    // Fade-out
    bool fadeOutActive = false;
    int fadeOutFrame = 0;
    int fadeOutFrames = 30;
    GAME_STATE_TYPE fadeTarget = GAME_STATE_TYPE::MENU;

    // Fade-in 
    bool fadeInActive = false;
    int fadeInFrame = 0;
    int fadeInFrames = 30;

    // Holds a fully opaque frame on the first render after a state switch
    bool holdBlack = false;

    AEGfxVertexList* fadeQuadMesh = nullptr;

    void BuildFadeQuad();
    void DrawOverlay(float alpha);
};

#endif // FADE_MANAGER_H