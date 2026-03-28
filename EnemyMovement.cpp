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
    enemy->rb->velocity.x = dir * enemy->base.stats.speed;

    if (enemy->pos.x >= enemy->base.patrolEnd.x && enemy->movement.movingRight)
        enemy->movement.movingRight = false;
    else if (enemy->pos.x <= enemy->base.patrolStart.x && !enemy->movement.movingRight)
        enemy->movement.movingRight = true;
}

//Get Closest Node
Node* EnemyMovement::GetClosestNode(AEVec2 pos) {
    Node* closest = nullptr;
    float minDist = FLT_MAX;

    for (Node* node : allNodes) {
        float dx = node->position.x - pos.x;
        float dy = node->position.y - pos.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < minDist) {
            minDist = dist;
            closest = node;
        }
    }

    return closest;
}

//A* Pathfinding
std::vector<AEVec2> EnemyMovement::FindPath(AEVec2 start, AEVec2 target) {

    std::vector<AEVec2> path;

    Node* startNode = GetClosestNode(start);
    Node* endNode = GetClosestNode(target);

#if DEBUG_PATH
    std::cout << "\n[A*] ===== NEW PATH REQUEST =====\n";
    std::cout << "Start Pos: (" << start.x << ", " << start.y << ")\n";
    std::cout << "Target Pos: (" << target.x << ", " << target.y << ")\n";
#endif

    if (!startNode || !endNode) {
#if DEBUG_PATH
        std::cout << "[A*] ERROR: Start or End node is NULL\n";
#endif
        return path;
    }

#if DEBUG_PATH
    std::cout << "[A*] Start Node: (" << startNode->position.x << ", " << startNode->position.y << ")\n";
    std::cout << "[A*] End Node: (" << endNode->position.x << ", " << endNode->position.y << ")\n";
#endif

    if (startNode == endNode) {
#if DEBUG_PATH
        std::cout << "[A*] Start == End → No path needed\n";
#endif
        return path;
    }

    std::vector<Node*> openList;
    std::vector<Node*> closedList;

    openList.push_back(startNode);

    startNode->gCost = 0.f;
    startNode->hCost = 0.f;
    startNode->parent = nullptr;

    while (!openList.empty()) {

        //Get lowest F cost node
        Node* current = openList[0];
        for (Node* node : openList) {
            if (node->FCost() < current->FCost())
                current = node;
        }

#if DEBUG_PATH
        std::cout << "[A*] Current Node: ("
            << current->position.x << ", " << current->position.y
            << ") F=" << current->FCost() << "\n";
#endif

        openList.erase(std::remove(openList.begin(), openList.end(), current), openList.end());
        closedList.push_back(current);

        if (current == endNode) {
#if DEBUG_PATH
            std::cout << "[A*] TARGET REACHED\n";
#endif
            break;
        }

        //Check neighbors
        for (Node* neighbor : current->neighbors) {

#if DEBUG_PATH
            std::cout << "   -> Checking Neighbor: ("
                << neighbor->position.x << ", " << neighbor->position.y << ")\n";
#endif

            if (std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end()) {
#if DEBUG_PATH
                std::cout << "      Skipped (already closed)\n";
#endif
                continue;
            }

            float dx = neighbor->position.x - current->position.x;
            float dy = neighbor->position.y - current->position.y;
            float newCost = current->gCost + sqrtf(dx * dx + dy * dy);

            bool inOpen = std::find(openList.begin(), openList.end(), neighbor) != openList.end();

            if (!inOpen) {
#if DEBUG_PATH
                std::cout << "      Added to open list\n";
#endif
                openList.push_back(neighbor);
            }
            else if (newCost >= neighbor->gCost) {
#if DEBUG_PATH
                std::cout << "      Skipped (worse path)\n";
#endif
                continue;
            }

            neighbor->gCost = newCost;

            float hx = neighbor->position.x - endNode->position.x;
            float hy = neighbor->position.y - endNode->position.y;
            neighbor->hCost = sqrtf(hx * hx + hy * hy);

            neighbor->parent = current;

#if DEBUG_PATH
            std::cout << "      Updated: G=" << neighbor->gCost
                << " H=" << neighbor->hCost
                << " F=" << neighbor->FCost() << "\n";
#endif
        }
    }

    //Reconstruct Path
    Node* current = endNode;

#if DEBUG_PATH
    std::cout << "[A*] Reconstructing path...\n";
#endif

    while (current && current->parent != nullptr) {
#if DEBUG_PATH
        std::cout << "   Path Node: ("
            << current->position.x << ", " << current->position.y << ")\n";
#endif
        path.push_back(current->position);
        current = current->parent;
    }

    std::reverse(path.begin(), path.end());

#if DEBUG_PATH
    std::cout << "[A*] Final Path Size: " << path.size() << "\n";
#endif

    return path;
}