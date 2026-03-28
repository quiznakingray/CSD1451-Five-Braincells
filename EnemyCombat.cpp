#include "EnemyCombat.h"
#include "PlayerGameObject.h"
#include <iostream>
#include <cstdlib>

void EnemyAttackPlayer(EnemyBase& enemy, Player& player)
{
	s32 dmg = rand() % enemy.stats.attack + 1;
	std::cout << "[EnemyCombat] Enemy attacks player for " << dmg << " damage\n";
}

bool IsEnemyDead(const EnemyBase& enemy)
{
	return enemy.stats.health <= 0;
}
