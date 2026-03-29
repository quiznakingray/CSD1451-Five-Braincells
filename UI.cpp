#include "UI.h"

#include "GameObjectManager.h"
#include "SpriteManager.h"
#include <vector>


ProgressBar::ProgressBar(ProgressBar&& other)
    : backgroundGO(other.backgroundGO), fillGO(other.fillGO), flashGO(other.flashGO)
    , background(other.background), fill(other.fill), flash(other.flash)
    , maxWidth(other.maxWidth), currentValue(other.currentValue)
    , maxValue(other.maxValue), flashTimer(other.flashTimer)
{
    other.backgroundGO = other.fillGO = other.flashGO = nullptr;
    other.background = other.fill = other.flash = nullptr;
}

ProgressBar::~ProgressBar()
{
    // If Free() was already called, all ptrs are null — safe no-op
    // If not, at least delete to avoid leak (won't remove from goList though)
    if (backgroundGO) { backgroundGO->Free(); delete backgroundGO; }
    if (fillGO) { fillGO->Free();       delete fillGO; }
    if (flashGO) { flashGO->Free();       delete flashGO; }
}


void ProgressBar::Init(float inMaxWidth, float inMaxValue,
    std::vector<GameObject*>& goList) // pass your scene's GO vector
{
    maxWidth = inMaxWidth;
    maxValue = inMaxValue;
    currentValue = inMaxValue;

    backgroundGO = new GameObject();
    fillGO = new GameObject();
    flashGO = new GameObject();

    background = backgroundGO->AddComponent(new Sprite());
    fill = fillGO->AddComponent(new Sprite());
    flash = flashGO->AddComponent(new Sprite());

    // register so UpdateGameObjects / RenderGameObjects picks them up
    AddGameObjectToVector(backgroundGO, goList);
    AddGameObjectToVector(fillGO, goList);
    AddGameObjectToVector(flashGO, goList);
}

void ProgressBar::Update(float newValue)
{
    float oldValue = currentValue;
    currentValue = AEClamp(newValue, 0.f, maxValue);

    if (currentValue != oldValue)
    {
        flashTimer = FLASH_DURATION;
        //flash->color = (currentValue > oldValue)
        //    ? AEVec4{ 1.f, 1.f, 1.f, 1.f }   // white — gain
        //: AEVec4{ 1.f, 0.f, 0.f, 1.f };   // red   — loss
    }
}

void ProgressBar::Render(AEVec2 anchorPos, float dt)
{
    float ratio = currentValue / maxValue;
    float fillWidth = maxWidth * ratio;
    float fillCenterX = anchorPos.x + (fillWidth * 0.5f) - (maxWidth * 0.5f);

    fill->size.x = fillWidth;
    fillGO->pos.x = fillCenterX;
    fillGO->pos.y = anchorPos.y;

    if (flashTimer > 0.f)
    {
        flashTimer -= dt;
        //flash->color.a = AEClamp(flashTimer / FLASH_DURATION, 0.f, 1.f);
        flash->size.x = fillWidth;
        flashGO->pos.x = fillCenterX;
        flashGO->pos.y = anchorPos.y;
        flashGO->isActive = true;
    }
    else
    {
        flashGO->isActive = false;
    }
}
void ProgressBar::Free(std::vector<GameObject*>& goList)
{
    // Remove from scene vector first, then delete
    auto remove = [&](GameObject* go)
        {
            if (!go) return;
            goList.erase(std::remove(goList.begin(), goList.end(), go), goList.end());
            go->Free();
            delete go;
        };

    remove(backgroundGO); backgroundGO = nullptr; background = nullptr;
    remove(fillGO);       fillGO = nullptr; fill = nullptr;
    remove(flashGO);      flashGO = nullptr; flash = nullptr;
}

