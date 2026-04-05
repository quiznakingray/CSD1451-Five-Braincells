#include "UIButton.h"
#include "AudioManager.h"

bool UIButton::IsClicked() const
{
    return isClicked;
}

bool UIButton::IsMouseOver() const
{
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    float halfW = AEGfxGetWindowWidth() * 0.5f;
    float halfH = AEGfxGetWindowHeight() * 0.5f;

    // Convert mouse pos to centered pixel space (0, 0)
    float px = mx - halfW; // convert pixel X to centered X
    float py = halfH - my; // convert pixel Y to centered Y

    AEVec2 camPos;
    AEGfxGetCamPosition(&camPos.x, &camPos.y);
    float screenX = x - camPos.x;
    float screenY = y - camPos.y;

    return (px > screenX - width * 0.5f && px < screenX + width * 0.5f &&
        py > screenY - height * 0.5f && py < screenY + height * 0.5f);
}

void UIButton::Init(float posX, float posY, float w, float h,
    const char* idlePath, const char* hoverPath)
{
    x = posX;
    y = posY;
    width = w;
    height = h;

    texIdle = AEGfxTextureLoad(idlePath);
    texHover = AEGfxTextureLoad(hoverPath);

    // Create quad mesh
    AEGfxMeshStart();

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);

    mesh = AEGfxMeshEnd();
}

void UIButton::Update()
{
    isHovered = IsMouseOver();
    isClicked = false;

    if (isHovered && AEInputCheckTriggered(AEVK_LBUTTON))
    {
        isClicked = true;
        AudioManager::GetInstance().PlaySFX("uiButtonClick"); // play button click sfx
    }
}

void UIButton::Update(float newX, float newY)
{
    x = newX;
    y = newY;
	Update(); // check hover and click with new position
}

void UIButton::Render()
{
    // Draw button texture based on idle or hover
    AEGfxTextureSet(isHovered ? texHover : texIdle, 0, 0);

    AEMtx33 scale, trans, final;
    AEMtx33Scale(&scale, width, height);
    AEMtx33Trans(&trans, x, y);
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // RESET
    AEMtx33 identity;
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);

    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
}

void UIButton::Free()
{
    if (texIdle)
    {
        AEGfxTextureUnload(texIdle);
        texIdle = nullptr;
    }

    if (texHover)
    {
        AEGfxTextureUnload(texHover);
        texHover = nullptr;
    }

    if (mesh)
    {
        AEGfxMeshFree(mesh);
        mesh = nullptr;
    }
}