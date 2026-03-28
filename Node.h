#ifndef NODE_H
#define NODE_H

#include "AEEngine.h"
#include <vector>

struct Node {
    AEVec2 position;
    std::vector<Node*> neighbors;

    float gCost, hCost;
    Node* parent;

    float FCost() const { return gCost + hCost; }

    // height difference helper
    float HeightDiff(Node* other) const {
        return other->position.y - position.y;
    }
};

#endif