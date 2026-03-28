#ifndef ENEMY_BASE_H
#define ENEMY_BASE_H

#include "AEEngine.h"

enum class EnemyType
{
	BASIC,
	MINI_BOSS
};

struct EnemyStats
{
	f32 speed;
	s32 health;
	s32 attack;
};

struct EnemyBase
{
	EnemyType type;
	EnemyStats stats;

	AEVec2 patrolStart;
	AEVec2 patrolEnd;

	bool isAlive;
};

void InitEnemyBase(EnemyBase& enemy, EnemyType type);
void PrintEnemyStats(const EnemyBase& enemy);

#endif
