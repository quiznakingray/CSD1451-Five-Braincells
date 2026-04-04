#ifndef ENEMY_MOVEMENT_H
#define ENEMY_MOVEMENT_H

#include "AEEngine.h"
#include <vector>
#include "Node.h"

struct EnemyGameObject;

struct EnemyMovement {
    bool movingRight = true;

    static void InitEnemyMovement(EnemyMovement& move);
    static void UpdateEnemyPatrol(EnemyGameObject* enemy);

    // A* Pathfinding
    static std::vector<AEVec2> FindPath(AEVec2 start, AEVec2 target);

    // Node system
    static std::vector<Node*> allNodes;
    static Node* GetClosestNode(AEVec2 pos);
};

#endif