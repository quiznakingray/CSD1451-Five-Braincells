#ifndef  PLAYER_GAME_OBJECT_H
#define  PLAYER_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"
#include "ParticleEffects.h"

enum class PLAYER_ACTION {
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

	
	//AEVec2 velocity{};

	// animation
	Animator* animator = nullptr;
	Animation* idleAnim = nullptr;
	Animation* runningAnim = nullptr;
	Animation* jumpAnim = nullptr;

	PLAYER_ACTION currentAction = PLAYER_ACTION::IDLE;
	PLAYER_ACTION prevAction = PLAYER_ACTION::IDLE;

	ParticleSystem hurtParticles;
	bool gotHurt = false;
	float hurtTimer = 0.0f;
	float hurtDuration = 2.0f;
	~Player();
	void Init() override;
	void Update() override;
	void Render() override;
	void Free() override;

	virtual void PlayerInput();
	void ReducePlayerHealth();
	virtual void PlayerAction();
	void ApplyDeceleration();

	virtual void TakeDamage(int amount);
	static void IncrementKills();
	virtual Animation* PlayerAnimation();
private:




	void ResetPlayer();
};

struct MeleePlayer : Player {

	bool inShieldAction = false; // will be true when Q is held down/enough stamina.
	bool shieldActive = false; // will be true when Q is held down/enough stamina.

	// shield duration
	float shieldTimer = 0.0f; // no. of seconds the shield has been held for this keypress
	bool shieldDepleted = false; // true after shield runs out, release Q for reset.

	Animation* shieldingAnim = nullptr;
	Animation* shieldAmin = nullptr;
	Animator* shieldBubbleAnimation = nullptr;
	Collider* shieldCollider = nullptr; 

	void Init() override;
	~MeleePlayer();
	void Update() override;
	void PlayerInput() override;
	void PlayerAction() override;
	Animation* PlayerAnimation() override;
	void TakeDamage(int amount) override;

};

struct RangePlayer : Player {

	Animation* aimingAnim = nullptr;
	Sprite* line = nullptr;
	Sprite::LinePoint* playerLinePos = nullptr;
	Sprite::LinePoint* aimLinePos = nullptr;
	
	~RangePlayer();

	float arrowTimer = 0.0f; // no. of seconds since last shot
	bool aiming = false;

	std::vector<Particle> particlePool;
	void Init() override;
	void Update() override;

	void PlayerInput() override;
	//void Render() override;
	void PlayerAction() override;
	Animation* PlayerAnimation() override;

};

struct Arrow : GameObject {

	RigidBody* rb = nullptr;
	f32 speed = 600.f;
	f32 timer = 0.0f;
	f32 lifetime = 5.f;
	int damage = 1;
	bool isEnemyProjectile = false;

	ParticleSystem particlePool;
	void Init() override;
	void Update() override;
	void Render() override;
	void ShootArrow(AEVec2 startPos, AEVec2 dir);
};

#endif // ! PLAYER_GAME_OBJECT_H
