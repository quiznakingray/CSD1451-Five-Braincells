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
	void StartPlayerCooldown(f64 dt);
};

#endif
