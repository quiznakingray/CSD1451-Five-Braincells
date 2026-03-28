#ifndef AUDIO_MENU_H
#define AUDIO_MENU_H

#include "UIButton.h"
#include "UISlider.h"

class AudioMenu
{
public:
    void Init();
    void Update();
    void Render();
    void Free();

    void Toggle();
    bool IsOpen() const;

private:
    bool isOpen = false;

    s8 font;
    f32 fontSize;

    // Normalize screen coords relative to screen center
    float NormalizeScreenX(float px);
    float NormalizeScreenY(float py);

    // Audio panel
    float panelX, panelY;
    float panelW, panelH;

    AEGfxTexture* panelTex;
    AEGfxVertexList* panelMesh;

    // Back button
    UIButton backButton;

    // Audio sliders
    float masterPos;
    float musicPos;
    float sfxPos;

    UISlider masterSlider;
    UISlider musicSlider;
    UISlider sfxSlider;

    void ApplyAudio();
};

#endif