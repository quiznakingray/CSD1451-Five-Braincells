#ifndef ENEMY_MOVEMENT_H
#define ENEMY_MOVEMENT_H

#include "AEEngine.h"
#include <vector>

struct EnemyGameObject;
struct EnemyMovement {
    bool movingRight = true;

    static void InitEnemyMovement(EnemyMovement& move);
    static void UpdateEnemyPatrol(EnemyGameObject* enemy, f32 dt);

    // A* Pathfinding
    static std::vector<AEVec2> FindPath(AEVec2 start, AEVec2 target);
};

#endif // ENEMY_MOVEMENT_H