#ifndef ENEMY_BASE_H
#define ENEMY_BASE_H

#include "AEEngine.h"
#include "PlayerGameObject.h"

enum class EnemyType
{
    BASIC_MELEE,
    BASIC_RANGED,
    MINI_BOSS_MELEE,
    MINI_BOSS_RANGED
};

struct EnemyStats
{
    f32 speed;
    s32 health;
    s32 maxHealth;
    s32 attack;

    f32 attackRange;
    f32 attackCooldown;
};

struct EnemyBase
{
    EnemyType type;
    EnemyStats stats;

    AEVec2 patrolStart;
    AEVec2 patrolEnd;

    f32 attackTimer;

    bool isAlive;
    bool canMove;

    Arrow* projectile = nullptr;
    virtual ~EnemyBase() {}
};

void InitEnemyBase(EnemyBase& enemy, EnemyType type);
void PrintEnemyStats(const EnemyBase& enemy);

#endif