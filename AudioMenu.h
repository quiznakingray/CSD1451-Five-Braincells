#ifndef AUDIO_MENU_H
#define AUDIO_MENU_H

#include "UIButton.h"
#include "UISlider.h"
#include "SingletonTemplate.h"

class AudioMenu : public Singleton<AudioMenu>
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

    s8 font{};
    const f32 fontSize = 80.0f;

    // Normalize screen coords relative to screen center
    float NormalizeScreenX(float px) const;
    float NormalizeScreenY(float py) const;

    // Audio panel
    float panelX = 0.0f, panelY = 0.0f;
    const float panelW = 700.0f, panelH = 700.0f;

    AEGfxTexture* panelTex = nullptr;
    AEGfxVertexList* panelMesh = nullptr;

    // Back button
    UIButton backButton;

    // Audio sliders
    float masterPos{};
    float musicPos{};
    float sfxPos{};

    UISlider masterSlider;
    UISlider musicSlider;
    UISlider sfxSlider;

    // Increase / Decrease buttons
    const float btnSize = 30.0f;
    UIButton masterMinus, masterPlus;
    UIButton musicMinus, musicPlus;
    UIButton sfxMinus, sfxPlus;

    void ApplyAudio();
};

#endif