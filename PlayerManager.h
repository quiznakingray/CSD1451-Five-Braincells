#ifndef PLAYER_MANAGER_H
#define PLAYER_MANAGER_H

#include "SingletonTemplate.h"
#include "PlayerGameObject.h"
struct PlayerManager : Singleton<PlayerManager>
{
	Player* meleePlayer;
	Player* rangedPlayer;

	Player* currentPlayer = meleePlayer;

	void Init();
	void Update();
	void Render();
	void Free();
};

#endif
