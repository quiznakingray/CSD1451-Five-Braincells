#ifndef UI_H
#define UI_H

#include "GameObjectManager.h"
struct ProgressBar
{
    GameObject* backgroundGO = nullptr;
    GameObject* fillGO = nullptr;
    GameObject* flashGO = nullptr;

    Sprite* background = nullptr;
    Sprite* fill = nullptr;
    Sprite* flash = nullptr;

    float maxWidth = 0.f;
    float currentValue = 0.f;
    float maxValue = 0.f;
    float flashTimer = 0.f;

    constexpr static float FLASH_DURATION = 0.3f;

    // No raw construction — use Init()
    ProgressBar() = default;

    // Non-copyable — owning raw pointers
    ProgressBar(const ProgressBar&) = delete;
    ProgressBar& operator=(const ProgressBar&) = delete;

    // Movable
    ProgressBar(ProgressBar&& other);

    ~ProgressBar();
    void Init(float inMaxWidth, float inMaxValue,
        std::vector<GameObject*>& goList);


    void Free(std::vector<GameObject*>& goList);

    void Update(float newValue);

    void Render(AEVec2 anchorPos, float dt);
};
#endif // !UI_H
