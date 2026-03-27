#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"

enum class PlayerAction {
	IDLE,
	RUNNING,
	JUMPING,
	CRATEINTERACT,
	ATTACKING,
	AIMING,
	SHIELDING,
};

struct Player : GameObject {

	Player() :GameObject()
	{
	}


	//STATE playerState = STATE::IDLE;
	f32 speed{};

	RigidBody* rb = nullptr;

	int defaultHealth = 5;
	int health = defaultHealth;
	
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
	void ReducePlayerHealth();
	virtual void PlayerAction();
private:


	void PlayerAnimation();
};

struct MeleePlayer : Player {

	bool shieldActive = false; // will be true when Q is held down/enough stamina.

	// shield duration
	float shieldTimer = 0.0f; // no. of seconds the shield has been held for this keypress
	bool shieldDepleted = false; // true after shield runs out, release Q for reset.

	Collider* shieldCollider = nullptr; 

	void Init() override;
	void Update() override;
	void PlayerAction() override;

};

struct RangePlayer : Player {

	Sprite* line = nullptr;
	Sprite::LinePoint* playerLinePos = nullptr;
	Sprite::LinePoint* aimLinePos = nullptr;

	float arrowTimer = 0.0f; // no. of seconds since last shot

	void Init() override;
	void Update() override;

	void PlayerAction() override;
};

struct Arrow : GameObject {

	RigidBody* rb = nullptr;
	f32 speed = 500.f;
	f32 timer = 0.0f;
	f32 lifetime = 5.f;

	int damage = 1;
	bool isEnemyProjectile = false; // for enemy projectiles set to true so shield collider will destroy

	void Init() override;
	void Update() override;
	void ShootArrow(AEVec2 startPos , AEVec2 dir, int dmg = 1);
};

#endif // ! PLAYER_GAME_OBJECT_H
