#ifndef ENEMY_COMBAT_H
#define ENEMY_COMBAT_H

#include "EnemyBase.h"

struct Player;

void EnemyAttackPlayer(EnemyBase& enemy, Player& player);
bool IsEnemyDead(const EnemyBase& enemy);

#endif
