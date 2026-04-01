#include "TextComponent.h"
#include "TextManager.h"
#include "GameObjectManager.h"

void Text::Render()
{
    f32 textW, textH;
    AEGfxGetPrintSize(TextManager::pFont, text, size * 1.f, &textW, &textH);
	//f32 width, height;
	//AEGfxGetPrintSize(TextManager::pFont, text, 1.f, &width, &height);
    f32 x, y;
	s32 windowWidth = AEGfxGetWindowWidth();
	s32 windowHeight = AEGfxGetWindowHeight();
    if(inWorldSpace)
    {

        // Get camera world position
        f32 camX, camY;
        AEGfxGetCamPosition(&camX, &camY);

        // world => camera space
        f32 relX = owner->pos.x + center.x - camX;
        f32 relY = owner->pos.y + center.y - camY;

        // Camera space => NDC (-1 to 1)
        x = relX / (windowWidth * 0.5f);
        y = relY / (windowHeight * 0.5f);

    }
    else {
        x = (owner->pos.x + center.x)/ (windowWidth * 0.5f) ;
        y = (owner->pos.y + center.y) / (windowHeight * 0.5f);
    }

    // Offset to center the text
    x -= textW * 0.5f;
    y -= textH * 0.5f;

    AEGfxPrint(TextManager::pFont, text, x, y , 1, 1, 1, 1, 1);
}

void Text::Free()
{
    textStr.clear();  // releases string heap memory
    text = "";
}
