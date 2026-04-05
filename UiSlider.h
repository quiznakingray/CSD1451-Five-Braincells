//---------------------------------------------------------
// author:    Nadyrah Tarmidi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef UI_SLIDER_H
#define UI_SLIDER_H

#include "AEEngine.h"
#include "AEGraphics.h"
#include "AEInput.h"

class UISlider
{
public:
    float value{}; // slider value

    void Init(float posX, float posY, float w, float h,
        float minVal, float maxVal, float startVal,
        const char* barPath, const char* handlePath);

    void Update();
    void Update(float newX, float newY);
    void Render();
    void Free();

private:
    s8 font{};
    const f32 fontSize = 80.0f;

    // Slider value text
    int prevValue{};
    bool showValue = false; // show slider value

    float displayTimer = 0.0f; // how long to show text
    const float displayDuration = 0.5f; // how long text stays visible

    // Normalize screen coords relative to screen center
    float NormalizeScreenX(float px) const;
    float NormalizeScreenY(float py) const;

    float HandleX(); // update slider handle pos x
    bool IsMouseOverHandle(); // check if mouse cursor over slider handle

    // Slider bar
    float barX{}, barY{};
    float barWidth{}, barHeight{};

    // Slider handle
    float handleX{}, handleY{};
    float handleWidth{}, handleHeight{};

    // Slider value
    float minValue{};
    float maxValue{};
    bool isDragging = false; // check if slider handle is being dragged

    AEGfxTexture* barTex = nullptr;
    AEGfxTexture* handleTex = nullptr;
    AEGfxVertexList* mesh = nullptr;
};

#endif