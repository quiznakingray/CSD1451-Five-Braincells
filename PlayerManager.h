#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "SingletonTemplate.h"
#include "PlayerGameObject.h"
enum class PLAYER_TYPE {
	MELEE,
	RANGE
};

//struct PlayerSaveData {
//	AEVec2 meleePos{};
//	AEVec2 rangedPos{};
//	bool hasSavedData = false;
//};

struct PlayerManager : public Singleton<PlayerManager>
{
	int maxStamina = 5;
	int stamina = 3;
	int currentStamina = 3;

	int maxHealth = 3;
	int currentHealth = 3;

	f32 staminaRegenTimer = 0.f;
	f32 staminaRegenDuration = 5.f;

	f32 healthRegenTimer = 0.f;
	f32 healthRegenDuration = 5.f;

	f32 playerSwitchingCooldown = 0.f;
	f32 playerSwitchingDuration = 2.f;
	bool canChangePlayer = true;

	MeleePlayer* meleePlayer{};
	RangePlayer* rangedPlayer{};

	static Arrow* rangePlayerArrow;
	Player* currentPlayer = meleePlayer;
	PLAYER_TYPE currentPlayerType = PLAYER_TYPE::MELEE;

	void Init();
	void Update();
	void Render();
	void Free();
	void Load();
	void ChangePlayer(PLAYER_TYPE type);
private:
	void RegenStamina(f32 dt);
	void RegenHealth(f32 dt);
	void StartPlayerCooldown(f32 dt);
};

#endif
