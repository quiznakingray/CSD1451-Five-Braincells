#include "EnemyCombat.h"
#include "PlayerGameObject.h"
#include "PlayerCombat.h"
#include <iostream>
#include <cstdlib>

void EnemyAttackPlayer(EnemyStats& enemyStats, PlayerStats& playerStats)
{
    s32 dmg = rand() % enemyStats.attack + 1;
    playerStats.health -= (f32)dmg;
    if (playerStats.health < 0.0f) playerStats.health = 0.0f;
    std::cout << "Enemy deals " << dmg << " damage\n";
}

bool IsEnemyDead(const EnemyBase& enemy)
{
    return enemyStats.health <= 0;
}

void TakeDamage(EnemyStats& enemyStats, s32 damage)
{
    enemy.stats.health -= damage;
    if (enemyStats.health < 0)
        enemyStats.health = 0;
    std::cout << "Player deals " << damage << " damage\n";
}

void InstantKill(EnemyStats& enemyStats)
{
    enemyStats.health = 0;
    std::cout << "[DEV] Enemy has been defeated\n";
}

void TurnOrder(EnemyStats& enemyStats, PlayerStats& playerStats, s32 playerDmg)
{
    bool playerFirst = (playerStats.speed >= enemyStats.speed);

    if (playerFirst)
    {
        std::cout << "[TurnOrder] Player is faster — player attacks first.\n"; // for debugging, remove when game is ready for submission
        TakeDamage(enemy, playerDmg);
        if (!IsEnemyDead(enemy))
            EnemyAttackPlayer(enemy, player);
    }
    else
    {
        std::cout << "[TurnOrder] Enemy is faster — enemy attacks first.\n"; // for debugging, remove when game is ready for submission
        EnemyAttackPlayer(enemy, player);
        if (player.health > 0.0f)
            TakeDamage(enemy, playerDmg);
    }
}