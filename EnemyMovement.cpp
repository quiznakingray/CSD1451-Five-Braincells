//---------------------------------------------------------
// author:    Nadyrah Tarmidi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "EnemyMovement.h"
#include "EnemyGameObject.h"
#include <algorithm>
#include <cmath>

#define DEBUG_PATH 0

std::vector<Node*> EnemyMovement::allNodes;

void EnemyMovement::InitEnemyMovement(EnemyMovement& move) {
    move.movingRight = true;
}

void EnemyMovement::UpdateEnemyPatrol(EnemyGameObject* enemy) {
    f32 dir = enemy->movement.movingRight ? 1.f : -1.f;
    enemy->rb->velocity.x = dir * enemy->base.stats.movementSpeed;

    if (enemy->pos.x >= enemy->base.patrolEnd.x && enemy->movement.movingRight)
        enemy->movement.movingRight = false;
    else if (enemy->pos.x <= enemy->base.patrolStart.x && !enemy->movement.movingRight)
        enemy->movement.movingRight = true;
}

// Node helpers
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
Node* EnemyMovement::GetClosestNode(AEVec3 pos) {
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

// Pathfinding
std::vector<AEVec2> EnemyMovement::FindPath(AEVec2 start, AEVec3 target) {
    std::vector<AEVec2> path;

    if (allNodes.empty()) return path;

    Node* startNode = GetClosestNode(start);
    Node* endNode = GetClosestNode(target);

    if (!startNode || !endNode || startNode == endNode) return path;

    // Reset all nodes
    for (Node* n : allNodes) n->ResetAll();

    startNode->actualCost = 0.f;
    startNode->estimatedCost = static_cast<float>(sqrtf(static_cast<float>(pow(startNode->position.x - endNode->position.x, 2)) +
        static_cast<float>(pow(startNode->position.y - endNode->position.y, 2))));
    startNode->UpdateTotalCost();

    std::vector<Node*> openList{ startNode };
    std::vector<Node*> closedList;

    bool reachedEnd = false;

    while (!openList.empty()) {
        // Pick node with lowest totalCost
        Node* currentNode = openList[0];
        for (Node* node : openList) {
            if (node->totalCost < currentNode->totalCost)
                currentNode = node;
        }

        openList.erase(std::remove(openList.begin(), openList.end(), currentNode), openList.end());
        closedList.push_back(currentNode);

        if (currentNode == endNode) {
            reachedEnd = true;
            break;
        }

        for (Node* neighbor : currentNode->neighbors) {
            if (!neighbor->walkable) continue;
            if (std::find(closedList.begin(), closedList.end(), neighbor) != closedList.end()) continue;

            float newCost = static_cast<float>(currentNode->actualCost + sqrtf(static_cast<float>(pow(neighbor->position.x - currentNode->position.x, 2)) +
                static_cast<float>(pow(neighbor->position.y - currentNode->position.y, 2))));

            bool inOpen = std::find(openList.begin(), openList.end(), neighbor) != openList.end();
            if (inOpen && newCost >= neighbor->actualCost) continue;

            neighbor->actualCost = newCost;
            neighbor->estimatedCost = static_cast<float>(sqrtf(static_cast<float>(pow(neighbor->position.x - endNode->position.x, 2)) +
                static_cast<float>(pow(neighbor->position.y - endNode->position.y, 2))));
            neighbor->parent = currentNode;
            neighbor->UpdateTotalCost();

            if (!inOpen) openList.push_back(neighbor);
        }
    }

    if (!reachedEnd) return path;

    // Reconstruct path
    Node* currentEndNode = endNode;
    while (currentEndNode && currentEndNode->parent != nullptr) {
        path.push_back(currentEndNode->position);
        currentEndNode = currentEndNode->parent;
    }
    std::reverse(path.begin(), path.end());

    return path;
}