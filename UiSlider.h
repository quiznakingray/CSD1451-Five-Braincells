#ifndef UI_SLIDER_H
#define UI_SLIDER_H

#include "AEEngine.h"
#include "AEGraphics.h"
#include "AEInput.h"

class UISlider
{
public:
    float value; // slider value

    void Init(float posX, float posY, float w, float h,
        float minVal, float maxVal, float startVal,
        const char* barPath, const char* handlePath);

    void Update();
    void Render();
    void Free();

private:
    s8 font;
    f32 fontSize;

    // Slider value text
    int prevValue;
    bool showValue; // show slider value

    float displayTimer; // how long to show text
    float displayDuration; // how long text stays visible

    // Normalize screen coords relative to screen center
    float NormalizeScreenX(float px);
    float NormalizeScreenY(float py);

    float HandleX(); // update slider handle pos x
    bool IsMouseOverHandle(); // check if mouse cursor over slider handle

    // Slider bar
    float barX, barY;
    float barWidth, barHeight;

    // Slider handle
    float handleX, handleY;
    float handleWidth, handleHeight;

    // Slider value
    float minValue;
    float maxValue;
    bool isDragging; // check if slider handle is being dragged

    AEGfxTexture* barTex;
    AEGfxTexture* handleTex;
    AEGfxVertexList* mesh;
};

#endif