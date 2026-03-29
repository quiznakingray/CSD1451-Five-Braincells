#include "EnemyBase.h"
#include <iostream>

void InitEnemyBase(EnemyBase& enemy, EnemyType type)
{
    enemy.type = type;
    enemy.isAlive = true;
    enemy.attackTimer = 0.f;

    switch (type)
    {
    case EnemyType::BASIC_MELEE:
        enemy.stats = { 80.f, 50, 50, 2, 150.f, 5.f };
        enemy.canMove = true;
        break;

    case EnemyType::BASIC_RANGED:
        enemy.stats = { 60.f, 40, 40, 1, 600.f, 2.0f };
        enemy.canMove = true;
        enemy.projectile = new Arrow();
        enemy.projectile->Init();
        break;

    case EnemyType::MINI_BOSS_MELEE:
        enemy.stats = { 0.f, 200, 200, 3, 200.f, 1.5f };
        enemy.canMove = false;
        break;

    case EnemyType::MINI_BOSS_RANGED:
        enemy.stats = { 0.f, 150, 150, 2, 700.f, 2.5f };
        enemy.canMove = false;
        enemy.projectile = new Arrow();
        enemy.projectile->Init();
        break;
    }

    PrintEnemyStats(enemy);
}

void PrintEnemyStats(const EnemyBase& enemy)
{
    std::cout << "Speed: " << enemy.stats.speed
        << " Health: " << enemy.stats.health
        << " Attack: " << enemy.stats.attack << std::endl;
}