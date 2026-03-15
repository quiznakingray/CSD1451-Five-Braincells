#ifndef PLAYER_COMBAT_H
#define PLAYER_COMBAT_H

#include "AEEngine.h"

// player structure
struct PlayerStats {
    f32 speed;
    s32 health;
    s32 maxhealth;
    s32 attack;
    const char* playername;
};

// abilities
struct AbilityButton {
    float x, y;
    float width, height;
    const char* name;
    int currentUses;
    int maxUses;
    float r, g, b;
    bool isHovered;
    bool isEnabled;
};
    struct PlayerBase;




    void GameStateLoad();
    void GameStateInit();
    void GameStateUpdate();
    void GameStateDraw();
    void GameStateFree();
    void GameStateUnload();

#endif

