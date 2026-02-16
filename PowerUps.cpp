#include "PowerUps.h"
#include <iostream>
#include <cstdlib>

PowerUpType GetRandomPowerUp()
{
	return static_cast<PowerUpType>(rand() % 3);
}

void ApplyPowerUp(PowerUpType type)
{
	switch (type)
	{
	case PowerUpType::SPEED:
		std::cout << "[PowerUp] Speed increased\n";
		break;
	case PowerUpType::HEALTH:
		std::cout << "[PowerUp] Health increased\n";
		break;
	case PowerUpType::ATTACK:
		std::cout << "[PowerUp] Attack increased\n";
		break;
	}
}
