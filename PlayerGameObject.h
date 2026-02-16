#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "PhysicsManager.h"

struct Player : GameObject {

	Player() :GameObject()
	{
	}
	f32 speed{};

	RigidBody* rb = nullptr;
	//AEVec2 velocity{};

	void PlayerInput();
	void Init() override;
	void Update() override;
};


#endif // ! PLAYER_GAME_OBJECT_H
