#ifndef ENEMY_GAME_OBJECT_H
#define ENEMY_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "EnemyBase.h"
#include "EnemyMovement.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"
#include "MapManager.h"

struct EnemyGameObject : GameObject {
    bool isGrounded = false;

    void Jump(float force);
    bool CheckGrounded();

    EnemyBase base;
    EnemyMovement movement;
    RigidBody* rb = nullptr;

    Animator* animator = nullptr;
    Animation* patrolAnim = nullptr;
    Animation* chaseAnim = nullptr;
    Animation* attackAnim = nullptr;

    ~EnemyGameObject();
    void Init(EnemyType type, Tile* spawnTile);
    void Update() override;
    void Render() override;

    void Patrol(f64 dt);
    void FollowPlayer(AEVec2 playerPos, f64 dt);
    void UpdateAnimation();
};

#endif