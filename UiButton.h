#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "AEEngine.h"
#include "AEGraphics.h"
#include "AEInput.h"
#include "AudioManager.h"

class UIButton
{
public:
    void Init(float posX, float posY, float w, float h,
        const char* idlePath, const char* hoverPath);

    void Update();
    void Render();
    void Free();

private:
    float x, y;
    float width, height;

    bool isHovered;
    bool isClicked;

    bool IsMouseOver(); // check if mouse cursor over button

    AEGfxTexture* texIdle;
    AEGfxTexture* texHover;
    AEGfxVertexList* mesh;
};

#endif