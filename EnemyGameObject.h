#ifndef ENEMY_GAME_OBJECT_H
#define ENEMY_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "EnemyBase.h"
#include "EnemyMovement.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"
#include "MapManager.h"

enum class EnemyState {
    IDLE,
    WALK,
    ATTACK
};

struct EnemyGameObject : GameObject {
    EnemyBase base;
    EnemyMovement movement;
    RigidBody* rb = nullptr;

    Animator* animator = nullptr;
    Animation* idleAnim = nullptr;
    Animation* walkAnim = nullptr;

    EnemyState currentState = EnemyState::IDLE;

    void Init(EnemyType type, Tile* spawnTile);
    void Update() override;
    void Render() override;

    void Patrol(f32 dt);
    void FollowPlayer(AEVec2 playerPos, f32 dt);
    void UpdateAnimation();
};

#endif