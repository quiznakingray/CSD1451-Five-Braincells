//---------------------------------------------------------
// author:    Nadyrah Tarmidi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef ENEMY_GAME_OBJECT_H
#define ENEMY_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "EnemyBase.h"
#include "EnemyMovement.h"
#include "PhysicsManager.h"
#include "AnimatorComponent.h"
#include "MapManager.h"
#include "AEVec3.h"
#include "ParticleEffects.h"

struct EnemyGameObject : GameObject {
    bool isGrounded = false;
    float damageTextDuration = 1.f;
    void Jump(float force);
    bool CheckGrounded();

    EnemyBase base;
    EnemyMovement movement;
    RigidBody* rb = nullptr;
    
	//take dmg 
    Text* healthText = nullptr;
    std::vector<std::pair<Text*, float>> hurtTexts;
	ParticleSystem hitEffect;

    Animator* animator = nullptr;
    Animation* patrolAnim = nullptr;
    Animation* chaseAnim = nullptr;
    Animation* attackAnim = nullptr;
    Animation* attackingAnim = nullptr;

    GameObject* healthBarBG = nullptr;
    GameObject* healthBarFG = nullptr;
    std::vector<GameObject*> healthBarObjects;

    ~EnemyGameObject();
    void Init(EnemyType type, Tile* spawnTile);
    void Update() override;
    void Render() override;
	void Free() override;

    void Patrol();
    void FollowPlayer(AEVec3 playerPos);
    void UpdateAnimation();
    void InitHealthBar();
    void UpdateHealthBar();

private:
    float walkSfxTimer = 0.f;
    float walkSfxInterval = 0.4f;

    void HandleWalkSFX(float dt);
};

#endif