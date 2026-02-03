#ifndef ENEMY_GAME_OBJECT_H
#define ENEMY_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "EnemyBase.h"
#include "EnemyMovement.h"
#include "PhysicsManager.h"
struct EnemyGameObject : GameObject
{
	EnemyBase base;
	EnemyMovement movement;

	RigidBody* rb = nullptr;

	void Init() override;
	void Update() override;
};

#endif
