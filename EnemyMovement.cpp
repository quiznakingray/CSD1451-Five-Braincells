#include "EnemyMovement.h"
#include "EnemyGameObject.h"
#include <queue>
#include <iostream>
#include <cmath>

void EnemyMovement::InitEnemyMovement(EnemyMovement& move) {
    move.movingRight = true;
}

void EnemyMovement::UpdateEnemyPatrol(EnemyGameObject* enemy, f32 dt) {
    f32 dir = enemy->movement.movingRight ? 1.f : -1.f;
    enemy->rb->velocity.x = dir * enemy->base.stats.speed * dt;

    if (enemy->pos.x >= enemy->base.patrolEnd.x && enemy->movement.movingRight)
        enemy->movement.movingRight = false;
    else if (enemy->pos.x <= enemy->base.patrolStart.x && !enemy->movement.movingRight)
        enemy->movement.movingRight = true;
}

// Simplified A* for demonstration
std::vector<AEVec2> EnemyMovement::FindPath(AEVec2 start, AEVec2 target) {
    std::vector<AEVec2> path;
    float dx = target.x - start.x;
    int steps = (int)fabs(dx / 10.f);
    float step = dx / steps;

    for (int i = 1; i <= steps; ++i) {
        path.push_back({ start.x + step * i, start.y });
    }

    return path;
}