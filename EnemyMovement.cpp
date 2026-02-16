#include "EnemyMovement.h"
#include "EnemyGameObject.h"
#include <iostream>

void InitEnemyMovement(EnemyMovement& move)
{
	move.movingRight = true;
	std::cout << "[EnemyMovement] Patrol initialized\n";
}

void UpdateEnemyPatrol(EnemyGameObject* enemy, f32 dt)
{
	bool wasMovingRight = enemy->movement.movingRight;

	f32 dir = enemy->movement.movingRight ? 1.f : -1.f;
	enemy->rb->velocity.x += dir * enemy->base.stats.speed * dt;

	if (enemy->pos.x >= enemy->base.patrolEnd.x && enemy->rb->velocity.x > 0)
	{
		enemy->movement.movingRight = false;
		enemy->rb->velocity.x = 0;
	}
	else if (enemy->pos.x <= enemy->base.patrolStart.x && enemy->rb->velocity.x < 0)
	{
		enemy->movement.movingRight = true;
		enemy->rb->velocity.x = 0;
	}

	if (enemy->movement.movingRight)
		std::cout << "[EnemyMovement] Turn right\n";
	else
		std::cout << "[EnemyMovement] Turn left\n";
}

void EnemyPatrol(EnemyGameObject* enemy)
{

}
