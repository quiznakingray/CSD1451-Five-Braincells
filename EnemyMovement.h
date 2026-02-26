#ifndef ENEMY_MOVEMENT_H
#define ENEMY_MOVEMENT_H

#include "AEEngine.h"
#include "EnemyBase.h"

struct EnemyGameObject;
struct EnemyMovement
{
	bool movingRight = false;
};

void InitEnemyMovement(EnemyMovement& move);
void UpdateEnemyPatrol(EnemyGameObject* enemy, f32 dt);

#endif
