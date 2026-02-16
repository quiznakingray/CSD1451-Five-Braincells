#include "EnemyBase.h"
#include <iostream>

void InitEnemyBase(EnemyBase& enemy, EnemyType type)
{
	enemy.type = type;
	enemy.isAlive = true;

	if (type == EnemyType::BASIC)
	{
		enemy.stats.speed = 30.f;
		enemy.stats.health = 50;
		enemy.stats.attack = 5;
	}
	else // MINI_BOSS
	{
		enemy.stats.speed = 20.f;
		enemy.stats.health = 150;
		enemy.stats.attack = 15;
	}

	std::cout << "[EnemyBase] Enemy initialised\n";
	PrintEnemyStats(enemy);
}

void PrintEnemyStats(const EnemyBase& enemy)
{
	std::cout << "Speed: " << enemy.stats.speed
		<< " Health: " << enemy.stats.health
		<< " Attack: " << enemy.stats.attack << std::endl;
}
