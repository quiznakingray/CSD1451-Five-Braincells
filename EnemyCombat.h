//---------------------------------------------------------
// author:    Nadyrah Tarmidi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef ENEMY_COMBAT_H
#define ENEMY_COMBAT_H

#include "EnemyBase.h"
#include "EnemyGameObject.h"

struct Player;

void EnemyAttackPlayer(EnemyBase& enemy, Player& player, AEVec2& enemyPos, float dt);
bool IsEnemyDead(EnemyBase& enemy);

void EnemyTakeDamage(EnemyGameObject& enemy, int damage);

#endif