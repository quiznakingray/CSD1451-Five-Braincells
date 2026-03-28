#include "AEEngine.h"
#include "PlayerCombat.h"
#include "GameStateManager.h"
#include "EndMenu.h"

// consts
const float WINDOW_WIDTH = 1600.0f;
const float WINDOW_HEIGHT = 900.0f;

// Global variables
AbilityButton abilities[4];
PlayerStats playerStats;
AEGfxVertexList* squareMesh;
AEGfxVertexList* circleMesh;
int selectedAbility = -1;
// color from rgba
unsigned long ColorToHex(float r, float g, float b, float a) {
    unsigned char red = (unsigned char)(r * 255);
    unsigned char green = (unsigned char)(g * 255);
    unsigned char blue = (unsigned char)(b * 255);
    unsigned char alpha = (unsigned char)(a * 255);
    return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

// colored square mesh
AEGfxVertexList* CreateColoredSquareMesh(float width, float height, float r, float g, float b, float a) {
    unsigned long color = ColorToHex(r, g, b, a);

    AEGfxMeshStart();

    float halfW = width / 2.0f;
    float halfH = height / 2.0f;

    AEGfxTriAdd(
        -halfW, -halfH, color, 0.0f, 0.0f,
        halfW, -halfH, color, 0.0f, 0.0f,
        -halfW, halfH, color, 0.0f, 0.0f
    );
    AEGfxTriAdd(
        halfW, -halfH, color, 0.0f, 0.0f,
        halfW, halfH, color, 0.0f, 0.0f,
        -halfW, halfH, color, 0.0f, 0.0f
    );

    return AEGfxMeshEnd();
}

// circle mesh
AEGfxVertexList* CreateCircleMesh(float radius, int segments, float r, float g, float b, float a) {
    unsigned long color = ColorToHex(r, g, b, a);

    AEGfxMeshStart();

    for (int i = 0; i < segments; i++) {
        float angle1 = (i * 2.0f * PI) / segments;
        float angle2 = ((i + 1) * 2.0f * PI) / segments;

        float x1 = radius * AECos(angle1);
        float y1 = radius * AESin(angle1);
        float x2 = radius * AECos(angle2);
        float y2 = radius * AESin(angle2);

        AEGfxTriAdd(
            0.0f, 0.0f, color, 0.0f, 0.0f,
            x1, y1, color, 0.0f, 0.0f,
            x2, y2, color, 0.0f, 0.0f
        );
    }

    return AEGfxMeshEnd();
}

// End Menu show up when player is dead
void CheckPlayerDeath() {
    if (playerStats.health <= 0) {
        EndMenu::isWin = false;
        EndMenu::isActive = true;
    }
}

// check if point is inside rectangle
bool isPointInRect(float px, float py, float rx, float ry, float width, float height) {
    return px >= rx - width / 2 && px <= rx + width / 2 &&
        py >= ry - height / 2 && py <= ry + height / 2;
}

void GameStateLoad() {
    squareMesh = CreateColoredSquareMesh(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f);
    circleMesh = CreateCircleMesh(1.0f, 32, 1.0f, 1.0f, 1.0f, 1.0f);

    // init font system and load font
}

void GameStateInit() {

    // init playerStats
    playerStats.health = 100.0f;
    playerStats.speed = 50.0f;
    playerStats.attack = 100.0f;

    // init abilities (2x2 grid)
    float buttonWidth = 350.0f;
    float buttonHeight = 100.0f;
    float spacing = 20.0f;
    float startX = -buttonWidth / 2 - spacing / 2;
    float startY = -250.0f;

    // ability 1 - attack (enabled)
    abilities[0].x = startX;
    abilities[0].y = startY;
    abilities[0].width = buttonWidth;
    abilities[0].height = buttonHeight;
    abilities[0].name = "Attack";
    abilities[0].currentUses = 999;
    abilities[0].maxUses = 999;
    abilities[0].r = 0.96f;
    abilities[0].g = 0.4f;
    abilities[0].b = 0.4f;
    abilities[0].isHovered = true;
    abilities[0].isEnabled = true;

    // ability 2 - disabled
    abilities[1].x = startX + buttonWidth + spacing;
    abilities[1].y = startY;
    abilities[1].width = buttonWidth;
    abilities[1].height = buttonHeight;
    abilities[1].name = "Ability 2";
    abilities[1].currentUses = 0;
    abilities[1].maxUses = 0;
    abilities[1].r = 0.5f;
    abilities[1].g = 0.5f;
    abilities[1].b = 0.5f;
    abilities[1].isHovered = false;
    abilities[1].isEnabled = false;

    // ability 3 - disabled
    abilities[2].x = startX;
    abilities[2].y = startY - buttonHeight - spacing;
    abilities[2].width = buttonWidth;
    abilities[2].height = buttonHeight;
    abilities[2].name = "Ability 3";
    abilities[2].currentUses = 0;
    abilities[2].maxUses = 0;
    abilities[2].r = 0.5f;
    abilities[2].g = 0.5f;
    abilities[2].b = 0.5f;
    abilities[2].isHovered = false;
    abilities[2].isEnabled = false;

    // ability 4 - disabled
    abilities[3].x = startX + buttonWidth + spacing;
    abilities[3].y = startY - buttonHeight - spacing;
    abilities[3].width = buttonWidth;
    abilities[3].height = buttonHeight;
    abilities[3].name = "Ability 4";
    abilities[3].currentUses = 0;
    abilities[3].maxUses = 0;
    abilities[3].r = 0.5f;
    abilities[3].g = 0.5f;
    abilities[3].b = 0.5f;
    abilities[3].isHovered = false;
    abilities[3].isEnabled = false;
}

void GameStateUpdate() {
    // get mouse position
    s32 mouseX, mouseY;
    AEInputGetCursorPosition(&mouseX, &mouseY);
 
    // convert screen coordinates to world coordinates
    float worldX = (float)mouseX - WINDOW_WIDTH / 2.0f;
    float worldY = WINDOW_HEIGHT / 2.0f - (float)mouseY;

    // check hover state for abilities
    for (int i = 0; i < 4; i++) {
        if (abilities[i].isEnabled) {
            abilities[i].isHovered = isPointInRect(worldX, worldY,
                abilities[i].x, abilities[i].y,
                abilities[i].width, abilities[i].height);

            // check for click
            if (abilities[i].isHovered && AEInputCheckTriggered(AEVK_LBUTTON)) {
                selectedAbility = i;
            }
        }
        else {
            abilities[i].isHovered = false;
        }
    }

    if (AEInputCheckTriggered(AEVK_M))
    {
        next = GAME_STATE_TYPE::WORLD;
    }
}

void drawHealthBar(float x, float y, float health, float maxHealth, const char* name, bool isEnemy) {
    float barWidth = 200.0f;
    float barHeight = 10.0f;

    // background (white panel)
    AEGfxVertexList* panelMesh = CreateColoredSquareMesh(barWidth + 20, 60, 1.0f, 1.0f, 1.0f, 1.0f);
    AEMtx33 trans;
    AEMtx33Trans(&trans, x, y);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(panelMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(panelMesh);

    // heakth bar background (gray)
    AEGfxVertexList* barBgMesh = CreateColoredSquareMesh(barWidth, barHeight, 0.88f, 0.91f, 0.94f, 1.0f);
    AEMtx33Trans(&trans, x, y - 10);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(barBgMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(barBgMesh);

    // Health bar fill
    float healthPercent = health / maxHealth;
    float currentBarWidth = barWidth * healthPercent;
    float barOffsetX = x - (barWidth - currentBarWidth) / 2.0f;

    float r, g, b;
    if (healthPercent > 0.5f) {
        r = 0.28f; g = 0.73f; b = 0.47f; // Green
    }
    else if (healthPercent > 0.25f) {
        r = 0.93f; g = 0.79f; b = 0.29f; // Yellow
    }
    else {
        r = 0.96f; g = 0.4f; b = 0.4f; // Red
    }

    AEGfxVertexList* barFillMesh = CreateColoredSquareMesh(currentBarWidth, barHeight, r, g, b, 1.0f);
    AEMtx33Trans(&trans, barOffsetX, y - 10);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(barFillMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(barFillMesh);
}

void GameStateDraw() {
    AEGfxSetBackgroundColor(0.53f, 0.81f, 0.92f); // Sky blue
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);

    AEMtx33 scale, trans, transform;

    // Draw ground
    AEGfxVertexList* groundMesh = CreateColoredSquareMesh(WINDOW_WIDTH, 300, 0.56f, 0.93f, 0.56f, 1.0f);
    AEMtx33Trans(&trans, 0, -200);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(groundMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(groundMesh);

    // Draw playerStats (blue circle)
    AEGfxVertexList* playerStatssprite = CreateCircleMesh(50, 32, 0.3f, 0.69f, 1.0f, 1.0f);
    AEMtx33Trans(&trans, -400.0f, 50.0f);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(playerStatssprite, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(playerStatssprite);

    // Draw enemy (red circle)
    AEGfxVertexList* enemySprite = CreateCircleMesh(60, 32, 1.0f, 0.42f, 0.44f, 1.0f);
    AEMtx33Trans(&trans, 400.0f, 200.0f);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(enemySprite, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(enemySprite);

    // health bar
    drawHealthBar(450, -100, playerStats.health, playerStats.maxhealth, playerStats.playername, false);

    // Draw UI panel (dark gray background)
    AEGfxVertexList* uiPanelMesh = CreateColoredSquareMesh(WINDOW_WIDTH, 360, 0.18f, 0.22f, 0.28f, 1.0f);
    AEMtx33Trans(&trans, 0, -270);
    AEGfxSetTransform(trans.m);
    AEGfxMeshDraw(uiPanelMesh, AE_GFX_MDM_TRIANGLES);
    AEGfxMeshFree(uiPanelMesh);

    // Draw ability buttons
    for (int i = 0; i < 4; i++) {
        float brightness = (abilities[i].isEnabled && abilities[i].isHovered) ? 1.2f : 1.0f;
        float opacity = abilities[i].isEnabled ? 1.0f : 0.5f;

        float r = AEMin(abilities[i].r * brightness, 1.0f);
        float g = AEMin(abilities[i].g * brightness, 1.0f);
        float b = AEMin(abilities[i].b * brightness, 1.0f);

        // Button border (darker)
        AEGfxVertexList* borderMesh = CreateColoredSquareMesh(
            abilities[i].width + 4, abilities[i].height + 4,
            r * 0.7f, g * 0.7f, b * 0.7f, opacity);
        AEMtx33Trans(&trans, abilities[i].x, abilities[i].y);
        AEGfxSetTransform(trans.m);
        AEGfxMeshDraw(borderMesh, AE_GFX_MDM_TRIANGLES);
        AEGfxMeshFree(borderMesh);

        // Button background
        AEGfxVertexList* buttonMesh = CreateColoredSquareMesh(
            abilities[i].width, abilities[i].height, r, g, b, opacity);
        AEMtx33Trans(&trans, abilities[i].x, abilities[i].y);
        AEGfxSetTransform(trans.m);
        AEGfxMeshDraw(buttonMesh, AE_GFX_MDM_TRIANGLES);
        AEGfxMeshFree(buttonMesh);
    }
}

void GameStateUnload() {
    AEGfxMeshFree(squareMesh);
    AEGfxMeshFree(circleMesh);
}

void GameStateFree() {
}

//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPWSTR lpCmdLine,
//    _In_ int nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    AESysInit(hInstance, nCmdShow, (s32)WINDOW_WIDTH, (s32)WINDOW_HEIGHT, 1, 60, true, NULL);
//    AESysSetWindowTitle("battle ui");
//
//    AEInputInit();
//    AEFrameRateControllerInit(60);
//
//    GameStateLoad();
//    GameStateInit();
//
//    while (AESysDoesWindowExist()) {
//        AESysFrameStart();
//        AEInputUpdate();
//        AEFrameRateControllerStart();
//
//        GameStateUpdate();
//        GameStateDraw();
//
//        AEFrameRateControllerEnd();
//        AESysFrameEnd();
//
//        if (AEInputCheckTriggered(AEVK_ESCAPE))
//            break;
//    }
//
//    GameStateUnload();
//    GameStateFree();
//
//    AEInputExit();
//    AESysExit();
//
//    return 0;
//}