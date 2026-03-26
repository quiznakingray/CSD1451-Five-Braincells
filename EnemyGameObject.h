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
};

#endif
