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
    void Update(float newX, float newY);
    void Render();
    void Free();

    bool IsClicked() const;

private:
    float x{}, y{};
    float width{}, height{};

    bool isHovered = false;
    bool isClicked = false;

    bool IsMouseOver() const; // check if mouse cursor over button

    AEGfxTexture* texIdle = nullptr;
    AEGfxTexture* texHover = nullptr;
    AEGfxVertexList* mesh = nullptr;
};

#endif