#include "EnemyBase.h"

// Initialize enemy with proper stats
void InitEnemyBase(EnemyBase& enemy, EnemyType type)
{
    enemy.type = type;
    enemy.isAlive = true;
    enemy.timeSinceLastAttack = 0.f;

    switch (type)
    {
    case EnemyType::BASIC_MELEE:
        enemy.stats = { 80.f, 5, 5, 2, 150.f, 5.f };
        enemy.canMove = true;
        enemy.currentState = EnemyState::PATROL;
        break;

    case EnemyType::BASIC_RANGED:
        enemy.stats = { 60.f, 3, 3, 1, 600.f, 7.f };
        enemy.canMove = true;
        enemy.currentState = EnemyState::PATROL;
        enemy.projectile = new Arrow();

        break;

    case EnemyType::MINI_BOSS_MELEE:
        enemy.stats = { 0.f, 20, 20, 3, 200.f, 1.5f };
        enemy.canMove = false;
        enemy.currentState = EnemyState::IDLE;
        break;

    case EnemyType::MINI_BOSS_RANGED:
        enemy.stats = { 0.f, 10, 10, 2, 700.f, 2.5f };
        enemy.canMove = false;
        enemy.currentState = EnemyState::IDLE;
        enemy.projectile = new Arrow();
        break;
    }

    // Initial patrol target
    enemy.currentTarget = enemy.patrolStart;

    PrintEnemyStats(enemy);
}

// Print enemy stats for debugging
void PrintEnemyStats(const EnemyBase& enemy)
{
    std::cout << "Enemy Type: " << static_cast<int>(enemy.type)
        << " | State: " << static_cast<int>(enemy.currentState)
        << " | Speed: " << enemy.stats.movementSpeed
        << " | Health: " << enemy.stats.health << "/" << enemy.stats.maxHealth
        << " | Damage: " << enemy.stats.damage
        << " | Range: " << enemy.stats.attackRange
        << " | Cooldown: " << enemy.stats.attackCooldown
        << std::endl;
}