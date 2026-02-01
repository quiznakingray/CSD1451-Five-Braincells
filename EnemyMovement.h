#ifndef ENEMY_MOVEMENT_H
#define ENEMY_MOVEMENT_H

#include "AEEngine.h"
#include "EnemyBase.h"

struct EnemyMovement
{
	bool movingRight;
};

void InitEnemyMovement(EnemyMovement& move);
void UpdateEnemyPatrol(EnemyBase& enemy, EnemyMovement& move, AEVec2& pos, f32 dt);

#endif
