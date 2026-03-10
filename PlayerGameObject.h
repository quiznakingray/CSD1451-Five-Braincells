#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"

enum class PlayerAction {
	IDLE,
	RUNNING,
	JUMPING
};

struct Player : GameObject {

	Player() :GameObject()
	{
	}


	//STATE playerState = STATE::IDLE;
	f32 speed{};

	RigidBody* rb = nullptr;

	//AEVec2 velocity{};

	// animation
	Animator* animator = nullptr;
	Animation* idleAnim = nullptr;
	Animation* runningAnim = nullptr;

	Sprite* line = nullptr;
	Sprite::LinePoint* playerLinePos = nullptr;
	Sprite::LinePoint* aimLinePos = nullptr;
	void Init() override;
	void Update() override;

	void PlayerInput();
private:

	PlayerAction currentAction = PlayerAction::IDLE;

	void PlayerAction();
	void PlayerAnimation();
};


#endif // ! PLAYER_GAME_OBJECT_H
