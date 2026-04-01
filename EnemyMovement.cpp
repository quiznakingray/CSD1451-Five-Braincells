#include "EnemyMovement.h"
#include "EnemyGameObject.h"
#include <algorithm>
#include <cmath>

#define DEBUG_PATH 0

std::vector<Node*> EnemyMovement::allNodes;

void EnemyMovement::InitEnemyMovement(EnemyMovement& move) {
    move.movingRight = true;
}

void EnemyMovement::UpdateEnemyPatrol(EnemyGameObject* enemy, f64 dt) {
    f32 dir = enemy->movement.movingRight ? 1.f : -1.f;
    enemy->rb->velocity.x = dir * enemy->base.stats.movementSpeed;

    if (enemy->pos.x >= enemy->base.patrolEnd.x && enemy->movement.movingRight)
        enemy->movement.movingRight = false;
    else if (enemy->pos.x <= enemy->base.patrolStart.x && !enemy->movement.movingRight)
        enemy->movement.movingRight = true;
}

// ----------------------------
// Node helpers
// ----------------------------
Node* EnemyMovement::GetClosestNode(AEVec2 pos) {
    Node* closest = nullptr;
    float minDist = FLT_MAX;

    for (Node* node : allNodes) {
        if (!node->walkable) continue;

        float dx = node->position.x - pos.x;
        float dy = node->position.y - pos.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < minDist) {
            minDist = dist;
            closest = node;
        }
    }

#if DEBUG_PATH
    if (closest)
        std::cout << "[GetClosestNode] Pos(" << pos.x << "," << pos.y
        << ") -> Node(" << closest->position.x << "," << closest->position.y
        << ") dist=" << minDist << "\n";
#endif

    return closest;
}

// ----------------------------
// A* Pathfinding
// ----------------------------
std::vector<AEVec2> EnemyMovement::FindPath(AEVec2 start, AEVec2 target) {
    std::vector<AEVec2> path;

    if (allNodes.empty()) return path;

    Node* startNode = GetClosestNode(start);
    Node* endNode = GetClosestNode(target);

    if (!startNode || !endNode || startNode == endNode) return path;

    // ResetAll all nodes using Node's ResetAll()
    for (Node* n : allNodes) n->ResetAll();

    startNode->actualCost = 0.f;
    startNode->estimatedCost = sqrtf(pow(startNode->position.x - endNode->position.x, 2) +
        pow(startNode->position.y - endNode->position.y, 2));
    startNode->UpdateTotalCost();

    std::vector<Node*> openList{ startNode };
    std::vector<Node*> closedList;

    bool reachedEnd = false;

    while (!openList.empty()) {
        // Pick node with lowest totalCost
        Node* current = openList[0];
        for (Node* node : openList) {
            if (node->totalCost < current->totalCost)
                current = node;
        }

        openList.erase(std::remove(openList.begin(), openList.end(), current), openList.end());
        closedList.push_back(current);

        if (current == endNode) {
            reachedEnd = true;
            break;
        }

        for (Node* neighbor : current->neighbors) {
            if (!neighbor->walkable) continue;
            if (std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end()) continue;

            float newCost = current->actualCost + sqrtf(pow(neighbor->position.x - current->position.x, 2) +
                pow(neighbor->position.y - current->position.y, 2));

            bool inOpen = std::find(openList.begin(), openList.end(), neighbor) != openList.end();
            if (inOpen && newCost >= neighbor->actualCost) continue;

            neighbor->actualCost = newCost;
            neighbor->estimatedCost = sqrtf(pow(neighbor->position.x - endNode->position.x, 2) +
                pow(neighbor->position.y - endNode->position.y, 2));
            neighbor->parent = current;
            neighbor->UpdateTotalCost();

            if (!inOpen) openList.push_back(neighbor);
        }
    }

    if (!reachedEnd) return path;

    // Reconstruct path
    Node* current = endNode;
    while (current && current->parent != nullptr) {
        path.push_back(current->position);
        current = current->parent;
    }
    std::reverse(path.begin(), path.end());

    return path;
}