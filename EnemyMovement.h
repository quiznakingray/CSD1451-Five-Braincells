#ifndef ENEMY_MOVEMENT_H
#define ENEMY_MOVEMENT_H

#include "AEEngine.h"
#include <vector>
#include "Node.h"
#include "AEVec3.h"

struct EnemyGameObject;

struct EnemyMovement {
    bool movingRight = true;

    static void InitEnemyMovement(EnemyMovement& move);
    static void UpdateEnemyPatrol(EnemyGameObject* enemy);

    // A* Pathfinding
    static std::vector<AEVec2> FindPath(AEVec2 start, AEVec3 target);

    // Node system
    static std::vector<Node*> allNodes;
    static Node* GetClosestNode(AEVec2 pos);
    static Node* GetClosestNode(AEVec3 pos);
};

#endif