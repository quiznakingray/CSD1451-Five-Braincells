#ifndef POWER_UPS_H
#define POWER_UPS_H

enum class PowerUpType
{
	SPEED,
	HEALTH,
	ATTACK
};

PowerUpType GetRandomPowerUp();
void ApplyPowerUp(PowerUpType type);

#endif
