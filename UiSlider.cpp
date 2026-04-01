#include "UISlider.h"

float UISlider::NormalizeScreenX(float px) const
{
    return px / (AEGfxGetWindowWidth() * 0.5f);
}

float UISlider::NormalizeScreenY(float py) const
{
    return py / (AEGfxGetWindowHeight() * 0.5f);
}

float UISlider::HandleX()
{
    float ratio = (value - minValue) / (maxValue - minValue);
    return barX - barWidth / 2 + ratio * barWidth;
}

bool UISlider::IsMouseOverHandle()
{
    // Mouse pos
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    float halfW = AEGfxGetWindowWidth() * 0.5f;
    float halfH = AEGfxGetWindowHeight() * 0.5f;

    // Convert mouse pos to centered pixel space (0, 0)
    float px = mx - halfW; // convert pixel X to centered X
    float py = halfH - my; // convert pixel Y to centered Y
    float hx = HandleX();

    return (px > hx - 15 && px < hx + 15 &&
        py > barY - 15 && py < barY + 15);
}

void UISlider::Init(float posX, float posY, float w, float h,
    float minVal, float maxVal, float startVal,
    const char* barPath, const char* handlePath)
{
    // Font
    font = AEGfxCreateFont("Assets/liberation-mono.ttf", static_cast<int>(fontSize));

    // Slider bar
    barX = posX;
    barY = posY;
    barWidth = w;
    barHeight = h;

    // Slider value
    minValue = minVal;
    maxValue = maxVal;
    value = startVal;

    prevValue = static_cast<int>(value);

    // Slider handle
    handleX = HandleX();
    handleY = barY + 5.0f;
    handleWidth = barWidth * 0.05f;
    handleHeight = barHeight * 0.7f;

    // Load slider bar and handle textures
    barTex = AEGfxTextureLoad(barPath);
    handleTex = AEGfxTextureLoad(handlePath);

    // Create quad mesh
    AEGfxMeshStart();
    const float E = 0.001f;
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, E, 1.0f - E,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f - E, 1.0f - E,
        -0.5f, 0.5f, 0xFFFFFFFF, E, E);

    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f - E, 1.0f - E,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f - E, E,
        -0.5f, 0.5f, 0xFFFFFFFF, E, E);

    mesh = AEGfxMeshEnd();
}

void UISlider::Update()
{
    s32 mx, my;
    AEInputGetCursorPosition(&mx, &my);

    if (AEInputCheckTriggered(AEVK_LBUTTON) && IsMouseOverHandle())
        isDragging = true;

    if (AEInputCheckReleased(AEVK_LBUTTON))
        isDragging = false;

    // Updates slider handle value when dragged
    if (isDragging)
    {
        float halfW = AEGfxGetWindowWidth() * 0.5f;
        float px = mx - halfW; // convert mouse X to centered pixels

        float clamped = AEClamp(px, barX - barWidth / 2, barX + barWidth / 2);
        float ratio = (clamped - (barX - barWidth / 2)) / barWidth;

        value = minValue + ratio * (maxValue - minValue);
    }

    // Shows slider value
    int currentValue = static_cast<int>(value);

    // If value changed, reset timer
    if (currentValue != prevValue)
    {
        prevValue = currentValue;
        displayTimer = displayDuration; // reset timer
    }

    // Countdown timer every frame
    if (displayTimer > 0.0f)
    {
        displayTimer -= AEFrameRateControllerGetFrameTime();
        showValue = true;
    }
    else
    {
        showValue = false;
    }

    // Update slider handle pos x every frame
    handleX = HandleX();
}

void UISlider::Render()
{
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);
    // Draw slider value text
    if (showValue)
    {
        char buffer[16];

        int intValue = static_cast<int>(value);
        sprintf_s(buffer, "%d", intValue);

        // Test pos relative to slider handle
        float px = handleX;
        float py = handleY + handleHeight * 0.5f; // text value directly above slider handle

        // Convert to normalized coords
        float nx = NormalizeScreenX(px);
        float ny = NormalizeScreenY(py);

        // Draw text
        AEGfxPrint(font, buffer, nx, ny + 0.05f, fontSize * 0.005f, 1.0f, 1.0f, 1.0f, 1.0f);
    }

    // Slider bar
    AEGfxTextureSet(barTex, 0, 0);

    AEMtx33 scale, trans, final;
    AEMtx33Scale(&scale, barWidth, barHeight);
    AEMtx33Trans(&trans, barX, barY);
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // RESET EVERYTHING
    AEMtx33 identity;
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);

    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetColorToMultiply(1, 1, 1, 1);

    // Slider handle
    AEGfxTextureSet(handleTex, 0, 0);

    AEMtx33Scale(&scale, handleWidth, handleHeight);
    AEMtx33Trans(&trans, handleX, handleY);
    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    // RESET EVERYTHING
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);

    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
}

void UISlider::Free()
{
    if (barTex)
    {
        AEGfxTextureUnload(barTex);
        barTex = nullptr;
    }

    if (handleTex)
    {
        AEGfxTextureUnload(handleTex);
        handleTex = nullptr;
    }

    if (mesh)
    {
        AEGfxMeshFree(mesh);
        mesh = nullptr;
    }
}