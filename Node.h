#ifndef NODE_H
#define NODE_H

#include "AEEngine.h"
#include <vector>

struct Node {
    AEVec2 position{};
    std::vector<Node*> neighbors;

    // A* costs
    float actualCost = 0.0f;     // gCost
    float estimatedCost = 0.0f;  // hCost
    float totalCost = 0.0f;      // fCost

    Node* parent = nullptr;

    // State tracking
    bool inOpenSet = false;
    bool inClosedSet = false;

    // Optional: for obstacles
    bool walkable = true;

    // Compute total cost
    void UpdateTotalCost() {
        totalCost = actualCost + estimatedCost;
    }

    float TotalCost() const {
        return totalCost;
    }

    // ResetAll node before a new A* run
    void ResetAll() {
        actualCost = 0.0f;
        estimatedCost = 0.0f;
        totalCost = 0.0f;
        parent = nullptr;
        inOpenSet = false;
        inClosedSet = false;
    }

    // Height difference helper
    float HeightDiff(const Node* other) const {
        return other->position.y - position.y;
    }

    // Comparison (for priority queue)
    bool operator>(const Node& other) const {
        return totalCost > other.totalCost;
    }
};

#endif