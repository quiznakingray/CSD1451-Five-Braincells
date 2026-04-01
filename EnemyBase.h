#ifndef ENEMY_BASE_H
#define ENEMY_BASE_H

#include "AEEngine.h"
#include "PlayerGameObject.h"
#include <iostream>

// Types of enemies
enum class EnemyType
{
    BASIC_MELEE,
    BASIC_RANGED,
    MINI_BOSS_MELEE,
    MINI_BOSS_RANGED
};

// Enemy behavior states
enum class EnemyState
{
    PATROL,
    CHASE,
    ATTACK,
    IDLE // For mini-bosses that don't move
};

// Stats structure
struct EnemyStats
{
    f32 movementSpeed;
    s32 health;
    s32 maxHealth;
    s32 damage;
    f32 attackRange;
    f32 attackCooldown;
};

// Base enemy structure
struct EnemyBase
{
    EnemyType type;
    EnemyStats stats;

    EnemyState currentState;

    AEVec2 patrolStart;
    AEVec2 patrolEnd;
    AEVec2 currentTarget; // For patrol movement

    f32 timeSinceLastAttack;

    bool isAlive;
    bool canMove;

    Arrow* projectile = nullptr; // Only for ranged enemies

    // Constructor / Destructor
    EnemyBase() : projectile(nullptr), isAlive(true), canMove(true), timeSinceLastAttack(0.f), currentState(EnemyState::IDLE) {}
    ~EnemyBase() {}
};

// Functions
void InitEnemyBase(EnemyBase& enemy, EnemyType type);
void PrintEnemyStats(const EnemyBase& enemy);

#endif