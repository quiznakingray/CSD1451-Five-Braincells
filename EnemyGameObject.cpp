#include "EnemyGameObject.h"
#include "EnemyManager.h"
#include "EnemyCombat.h"
#include <iostream>
#include <cmath>

bool EnemyGameObject::CheckGrounded() {
    return (fabs(rb->velocity.y) < 0.01f);
}

void EnemyGameObject::Jump(float force) {
    if (CheckGrounded()) rb->velocity.y = force;
}

EnemyGameObject::~EnemyGameObject() {
    delete patrolAnim; patrolAnim = nullptr;
    delete chaseAnim; chaseAnim = nullptr;
    delete attackAnim; attackAnim = nullptr;
}

void EnemyGameObject::Init(EnemyType type, Tile* spawnTile) {
    InitEnemyBase(base, type);
    EnemyMovement::InitEnemyMovement(movement);

    if (!spawnTile) return;

    AEVec2Set(&pos, spawnTile->pos.x + 50, spawnTile->pos.y);
    pos.z = 1.f;
    AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);

    base.patrolStart = { spawnTile->pos.x - 100.f, spawnTile->pos.y - 100.f };
    base.patrolEnd = { spawnTile->pos.x + 150.f, spawnTile->pos.y - 100.f };

    // Setup animations
    Sprite* s = new Sprite();
    s->meshColor = (type == EnemyType::BASIC_MELEE) ? 0xFF0000FF : 0xFFFF0000;
    s->textureFileName = "Assets/SpriteSheets/Enemy_Basic_Melee_Idle.png";
    s->spriteSheet = Sprite::SpriteSheet(2, 7);
    s->spriteSheet.isSpriteSheet = true;

    Sprite* walk = new Sprite();
    walk->meshColor = (type == EnemyType::BASIC_RANGED) ? 0xFF0000FF : 0xFFFF0000;
    walk->textureFileName = "Assets/SpriteSheets/Enemy_Basic_Melee_Walk.png";
    walk->spriteSheet = Sprite::SpriteSheet(2, 7);
    walk->spriteSheet.isSpriteSheet = true;

    Sprite* attack = new Sprite();
    attack->meshColor = (type == EnemyType::BASIC_RANGED) ? 0xFF0000FF : 0xFFFF0000;
    attack->textureFileName = "Assets/SpriteSheets/Enemy_Basic_Melee_Attack.png";
    attack->spriteSheet = Sprite::SpriteSheet(2, 7);
    attack->spriteSheet.isSpriteSheet = true;

    patrolAnim = new Animation(s);
    patrolAnim->loopAnimation = true;
    patrolAnim->animationFPS = 10.f;

    chaseAnim = new Animation(walk);
    chaseAnim->loopAnimation = true;
    chaseAnim->animationFPS = 10.f;

    attackAnim = new Animation(attack);
    attackAnim->loopAnimation = true;
    attackAnim->animationFPS = 10.f;

    rb = AddComponent(new RigidBody());
    rb->type = RIGIDBODY_TYPE::KINEMATIC;

    animator = AddComponent(new Animator(patrolAnim));
    AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

    base.isAlive = true;
    GameObject::Init();
}

void EnemyGameObject::Update() {
    float dt = AEFrameRateControllerGetFrameTime();

    if (!EnemyManager::GetInstance().player) {
        UpdateAnimation();
        GameObject::Update();
        if (base.projectile) base.projectile->Update(); // Update arrow
        return;
    }

    AEVec2 playerPos = EnemyManager::GetInstance().GetPlayerPos();
    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    float distSq = dx * dx + dy * dy;
    float attackRangeSq = base.stats.attackRange * base.stats.attackRange;
    float detectionRangeSq = 500.f * 500.f;

    bool isMelee = (base.type == EnemyType::BASIC_MELEE || base.type == EnemyType::MINI_BOSS_MELEE);
    bool isRanged = (base.type == EnemyType::BASIC_RANGED || base.type == EnemyType::MINI_BOSS_RANGED);

    // State machine: ATTACK > CHASE > PATROL
    if (distSq <= attackRangeSq) {
        if (rb) rb->velocity.x = 0.f;
        base.currentState = EnemyState::ATTACK;
        EnemyAttackPlayer(base, *EnemyManager::GetInstance().player, pos, dt);
    }
    else if (distSq < detectionRangeSq && base.canMove) {
        if (isMelee) FollowPlayer(playerPos, dt);
        else if (isRanged) {
            const float tooClose = 150.f;
            if (distSq < tooClose * tooClose) rb->velocity.x = (dx > 0 ? -1 : 1) * base.stats.movementSpeed;
            else rb->velocity.x = 0.f;
            base.currentState = (distSq < tooClose * tooClose) ? EnemyState::CHASE : EnemyState::PATROL;
        }
    }
    else {
        if (rb) rb->velocity.x = 0.f;
        base.currentState = EnemyState::PATROL;
        if (base.canMove) Patrol(dt);
    }

    UpdateAnimation();
    GameObject::Update();
}

void EnemyGameObject::Patrol(f64 dt) {
    if (!base.canMove) return;
    EnemyMovement::UpdateEnemyPatrol(this, dt);
    base.currentState = EnemyState::PATROL;
}

void EnemyGameObject::FollowPlayer(AEVec2 playerPos, f64 dt) {
    if (!base.canMove) return;

    if (!EnemyMovement::allNodes.empty()) {
        std::vector<AEVec2> path = EnemyMovement::FindPath(pos, playerPos);
        for (AEVec2& node : path) {
            float dx = node.x - pos.x;
            float dy = node.y - pos.y;
            float distSq = dx * dx + dy * dy;
            if (distSq > 25.f) { // 5 units squared
                float dist = sqrtf(distSq);
                rb->velocity.x = dx / dist * base.stats.movementSpeed;
                if (dy > 20.f) Jump(300.f);
                base.currentState = EnemyState::CHASE;
                return;
            }
        }
    }

    float dx = playerPos.x - pos.x;
    float dy = playerPos.y - pos.y;
    float distSq = dx * dx + dy * dy;
    if (distSq > 0.0001f) {
        float dist = sqrtf(distSq);
        rb->velocity.x = dx / dist * base.stats.movementSpeed;
        if (dy > 20.f) Jump(300.f);
        base.currentState = EnemyState::CHASE;
    }
    else rb->velocity.x = 0.f;
}

void EnemyGameObject::UpdateAnimation() {
    if (!animator) return;
    Animation* anim = (base.currentState == EnemyState::PATROL) ? patrolAnim :
        (base.currentState == EnemyState::CHASE) ? chaseAnim :
        attackAnim;
    animator->PlayAnimation(anim);
}

void EnemyGameObject::Render() {
    GameObject::Render();
    if (base.projectile) base.projectile->Render();
}