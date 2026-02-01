#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"

struct Player : GameObject {

	Player() :GameObject()
	{
	}
	f32 speed{};
	AEVec2 velocity{};
	void Init() override;
	void Update() override;
};


#endif // ! PLAYER_GAME_OBJECT_H
