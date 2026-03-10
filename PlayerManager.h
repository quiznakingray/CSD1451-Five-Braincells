#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "SingletonTemplate.h"
#include "PlayerGameObject.h"
enum class PLAYER_TYPE {
	MELEE,
	RANGE
};
struct PlayerManager : Singleton<PlayerManager>
{
	MeleePlayer* meleePlayer;
	RangePlayer* rangedPlayer;

	static Arrow* rangePlayerArrow;
	Player* currentPlayer = meleePlayer;

	AEVec2 camPos{};
	f32 camLerpSpeed = 5.f;

	void Init();
	void Update();
	void Render();

	void ChangePlayer(PLAYER_TYPE type);
};

#endif
