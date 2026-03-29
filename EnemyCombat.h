#ifndef ENEMY_COMBAT_H
#define ENEMY_COMBAT_H

#include "EnemyBase.h"

struct Player;

void EnemyAttackPlayer(EnemyBase& enemy, Player& player, AEVec2& enemyPos, float dt);
bool IsEnemyDead(EnemyBase& enemy);

#endif