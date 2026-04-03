#include "LoadingScreen.h"
#include "AEEngine.h"

#include <string>
#include <array>

GAME_STATE_TYPE LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;

int LOADING_FRAMES = 60;

AEGfxTexture* loadingTexture = nullptr;
static AEGfxVertexList* mesh = nullptr;
int frameCount = 0;
int dotFrame = 0;
std::string loadingText = "Loading";
int fontId = 0;

const std::array<std::string, 4> dotStates = {
    "Loading",
    "Loading.",
    "Loading..",
    "Loading..."
};

void LoadingScreen::Load()
{
    loadingTexture = AEGfxTextureLoad("Assets/Environment/bg_castle.png");
}

void LoadingScreen::Init()
{
    frameCount = 0;
    dotFrame = 0;
    loadingText = "Loading";
    fontId = AEGfxCreateFont("Assets/liberation-mono.ttf", 60);

    AEGfxMeshStart();

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, -0.5f, 0xFFFFFFFF, 1, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0);

    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0, 1,
        0.5f, 0.5f, 0xFFFFFFFF, 1, 0,
        -0.5f, 0.5f, 0xFFFFFFFF, 0, 0);

    mesh = AEGfxMeshEnd();
}

void LoadingScreen::Update()
{
    ++frameCount;
    ++dotFrame;

    loadingText = dotStates[(dotFrame / 20) % 4];

    if (frameCount >= LOADING_FRAMES)
        GameStateManager::GetInstance().ChangeState(targetState);
}

void LoadingScreen::Render()
{
    AEGfxSetBackgroundColor(0.f, 0.f, 0.f);

    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetColorToMultiply(1, 1, 1, 1);
    AEGfxSetColorToAdd(0, 0, 0, 0);
    AEGfxSetTransparency(1.0f);

    AEGfxTextureSet(loadingTexture, 0, 0);

    // Scale to screen size
    float w = static_cast<float>(AEGfxGetWindowWidth());
    float h = static_cast<float>(AEGfxGetWindowHeight());

    AEMtx33 scale{}, trans{}, final{};

    AEMtx33Scale(&scale, w, h);     // full screen
    AEMtx33Trans(&trans, 0.f, 0.f); // centered

    AEMtx33Concat(&final, &trans, &scale);

    AEGfxSetTransform(final.m);
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

    AEMtx33 identity{};
    AEMtx33Identity(&identity);
    AEGfxSetTransform(identity.m);

    constexpr float textX = -0.2f;
    constexpr float textY = -0.05f;
    AEGfxPrint(fontId, loadingText.c_str(), textX, textY, 1.5f, 1.f, 1.f, 1.f, 1.f);
}

void LoadingScreen::Free()
{
    if (mesh)
    {
        AEGfxMeshFree(mesh);
        mesh = nullptr;
    }
}

void LoadingScreen::Unload()
{
    if (loadingTexture)
    {
        AEGfxTextureUnload(loadingTexture);
        loadingTexture = nullptr;
    }
    AEGfxDestroyFont(fontId);
}