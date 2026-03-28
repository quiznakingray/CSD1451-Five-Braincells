#ifndef ENEMY_GAME_OBJECT_H
#define ENEMY_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "EnemyBase.h"
#include "EnemyMovement.h"
#include "PhysicsManager.h"
#include "ParticleEffects.h"
struct EnemyGameObject : GameObject
{
	EnemyBase base{};
	EnemyMovement movement{};

	RigidBody* rb = nullptr;

	std::vector<Particle> particlePool;

	void Init() override;
	void Update() override;

	void Render() override;
#include "AnimatorComponent.h"
#include "MapManager.h"

enum class EnemyState {
    IDLE,
    WALK,
    ATTACK
};

struct EnemyGameObject : GameObject {
    bool isGrounded = false;

    void Jump(float force);
    bool CheckGrounded();

    EnemyBase base;
    EnemyMovement movement;
    RigidBody* rb = nullptr;

    Animator* animator = nullptr;
    Animation* idleAnim = nullptr;
    Animation* walkAnim = nullptr;

    EnemyState currentState = EnemyState::IDLE;
    ~EnemyGameObject();
    void Init(EnemyType type, Tile* spawnTile);
    void Update() override;
    void Render() override;

    void Patrol(f64 dt);
    void FollowPlayer(AEVec2 playerPos, f64 dt);
    void UpdateAnimation();
};

#endif