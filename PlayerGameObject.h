#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"

enum class PlayerAction {
	IDLE,
	RUNNING,
	JUMPING,
	CRATEINTERACT
	ATTACKING,
	AIMING,
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

	PlayerAction currentAction = PlayerAction::IDLE;
	PlayerAction prevAction = PlayerAction::IDLE;

	void Init() override;
	void Update() override;

	void PlayerInput();
	virtual void PlayerAction();
private:


	void PlayerAnimation();
};

struct MeleePlayer : Player {

	void Init() override;
};

struct RangePlayer : Player {

	Sprite* line = nullptr;
	Sprite::LinePoint* playerLinePos = nullptr;
	Sprite::LinePoint* aimLinePos = nullptr;

	void Init() override;
	void Update() override;

	void PlayerAction() override;
};

struct Arrow : GameObject {

	RigidBody* rb = nullptr;
	f32 speed = 500.f;
	f32 timer = 0.0f;
	f32 lifetime = 5.f;
	void Init() override;
	void Update() override;
	void ShootArrow(AEVec2 startPos , AEVec2 dir);
};
#endif // ! PLAYER_GAME_OBJECT_H
