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

private:
    // Audio panel
    float panelX, panelY;
    float panelW, panelH;

    AEGfxTexture* panelTex;
    AEGfxVertexList* panelMesh;

    // Back button
    UIButton backButton;

    // Audio sliders
    UISlider masterSlider;
    UISlider musicSlider;
    UISlider sfxSlider;

    void ApplyAudio();
};

#endif