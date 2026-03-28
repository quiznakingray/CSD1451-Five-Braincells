#include "TextComponent.h"
#include "GameObjectManager.h"

void Text::Render()
{
    f32 textW, textH;
    AEGfxGetPrintSize(TextManager::pFont, text, 1.f, &textW, &textH);
	//f32 width, height;
	//AEGfxGetPrintSize(TextManager::pFont, text, 1.f, &width, &height);
	s32 windowWidth = AEGfxGetWindowWidth();
	s32 windowHeight = AEGfxGetWindowHeight();

    // 1. Get camera world position
    f32 camX, camY;
    AEGfxGetCamPosition(&camX, &camY);

    // 2. World => camera space
    f32 relX = owner->pos.x + center.x - camX;
    f32 relY = owner->pos.y + center.y - camY;

    // 3. Camera space => NDC (-1 to 1)
    f32 ndcX = relX / (windowWidth * 0.5f);
    f32 ndcY = relY / (windowHeight * 0.5f);


    // 5. Offset to center the text
    ndcX -= textW * 0.5f;
    ndcY -= textH * 0.5f;

	AEGfxPrint(TextManager::pFont, text, ndcX, ndcY , 1, 1, 1, 1, 1);
}
