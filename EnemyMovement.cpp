#include "EnemyMovement.h"
#include <iostream>

void InitEnemyMovement(EnemyMovement& move)
{
	move.movingRight = true;
	std::cout << "[EnemyMovement] Patrol initialised\n";
}

void UpdateEnemyPatrol(EnemyBase& enemy, EnemyMovement& move, AEVec2& pos, f32 dt)
{
	bool wasMovingRight = move.movingRight;

	f32 dir = move.movingRight ? 1.f : -1.f;
	pos.x += dir * enemy.stats.speed * dt;

	if (pos.x >= enemy.patrolEnd.x)
	{
		move.movingRight = false;
	}
	else if (pos.x <= enemy.patrolStart.x)
	{
		move.movingRight = true;
	}

	if (move.movingRight)
		std::cout << "[EnemyMovement] Turn right\n";
	else
		std::cout << "[EnemyMovement] Turn left\n";
}
