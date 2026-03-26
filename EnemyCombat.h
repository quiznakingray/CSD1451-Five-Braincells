#ifndef ENEMY_COMBAT_H
#define ENEMY_COMBAT_H

#include "AEEngine.h"

struct EnemyStats {
    s32 health;
    s32 maxHealth;
    s32 attack;
    f32 speed;
    const char* name;
};

struct PlayerStats;

void EnemyAttackPlayer(EnemyStats& enemyStats, Player& player);
bool IsEnemyDead(const EnemyStats& enemyStats);
void TakeDamage(EnemyStats& enemyStats, s32 damage);
void InstantKill(EnemyStats& enemyStats);
void TurnOrder(EnemyStats& enemyStats, PlayerStats& playerStats, s32 playerDmg);

#endif