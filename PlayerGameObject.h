#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"
#include "ParticleEffects.h"

enum class PlayerAction {
	IDLE,
	RUNNING,
	JUMPING,
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

	void Init() override;
	void Update() override;

	void PlayerInput();
	PlayerAction currentAction = PlayerAction::IDLE;

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

	std::vector<Particle> particlePool;
	void Init() override;
	void Update() override;
	//void Render() override;
	void PlayerAction() override;
};

struct Arrow : GameObject {

	RigidBody* rb = nullptr;
	f32 speed = 500.f;
	f32 timer = 0.0f;
	f32 lifetime = 5.f;

	std::vector<Particle> particlePool;
	void Init() override;
	void Update() override;
	void Render() override;
	void ShootArrow(AEVec2 startPos , AEVec2 dir);
};
#endif // ! PLAYER_GAME_OBJECT_H
