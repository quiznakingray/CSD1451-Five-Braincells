#include "EnemyGameObject.h"
#include "EnemyManager.h"
#include <iostream>
#include "EnemyMovement.h"
#include "EnemyCombat.h"

bool EnemyGameObject::CheckGrounded() {
    // Replace with collision check if have
    return (fabs(rb->velocity.y) < 0.01f);
}

void EnemyGameObject::Jump(float force) {
    if (CheckGrounded()) {
        rb->velocity.y = force;
    }
}

EnemyGameObject::~EnemyGameObject()
{
    if (idleAnim) {
        delete idleAnim;
        idleAnim = nullptr;
    }
    if (walkAnim) {
        delete walkAnim;
        walkAnim = nullptr;
    }
    if (attackAnim) {
        delete attackAnim;
        attackAnim = nullptr;
    }
}

void EnemyGameObject::Init(EnemyType type, Tile* spawnTile) {
    InitEnemyBase(base, type);
    EnemyMovement::InitEnemyMovement(movement);

    if (!spawnTile) {
        //std::cout << "[EnemyGameObject] No spawn tile provided!\n";
        return;
    }

    // Position
    AEVec2Set(&pos, spawnTile->pos.x + 50, spawnTile->pos.y);
    pos.z = 1.f;

    // Scale
    AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);

    // Patrol points around spawn
    base.patrolStart = { spawnTile->pos.x - 100.f, spawnTile->pos.y - 100.f };
    base.patrolEnd = { spawnTile->pos.x + 150.f, spawnTile->pos.y - 100.f };

    // Sprite setup
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

    idleAnim = new Animation(s);
    idleAnim->loopAnimation = true;
    idleAnim->animationFPS = 10.f;
    walkAnim = new Animation(walk);
    walkAnim->loopAnimation = true;
    walkAnim->animationFPS = 10.f;
    attackAnim = new Animation(attack);
    attackAnim->loopAnimation = true;
    attackAnim->animationFPS = 10.f;

    // Add Rigidbody first
    rb = AddComponent(new RigidBody());
    rb->type = RIGIDBODY_TYPE::DYNAMIC;
    rb->mass = 10;

    // Add Animator after Rigidbody
    animator = AddComponent(new Animator(idleAnim));

    // Collider
    Collider* c = AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

    base.isAlive = true;

    // Register enemy last
    //EnemyManager::RegisterEnemy(this);

    //std::cout << "[EnemyGameObject] Initialized at (" << pos.x << ", " << pos.y << ")\n";
    GameObject::Init();
}

void EnemyGameObject::Update() {
    float dt = AEFrameRateControllerGetFrameTime();

    if (EnemyManager::GetInstance().player)
    {
        EnemyAttackPlayer(base, *EnemyManager::GetInstance().player, pos, dt);
    }

    if (base.canMove)
    {
        AEVec2 playerPos = EnemyManager::GetInstance().GetPlayerPos();

        float dx = playerPos.x - pos.x;
        float dy = playerPos.y - pos.y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < 500.f)
            FollowPlayer(playerPos, dt);
        else
            Patrol(dt);
    }
    else
    {
        if (rb) rb->velocity.x = 0;
    }

    UpdateAnimation();
    GameObject::Update();
}

void EnemyGameObject::Patrol(f64 dt) {
    if (currentState == EnemyState::IDLE || currentState == EnemyState::WALK) {
        EnemyMovement::UpdateEnemyPatrol(this, dt);
        currentState = EnemyState::WALK;
    }
}

void EnemyGameObject::FollowPlayer(AEVec2 playerPos, f64 dt) {
    // Call A* pathfinding to update velocity
    std::vector<AEVec2> path = EnemyMovement::FindPath(pos, playerPos);
    //std::cout << "[Enemy] Following Player, Path size: " << path.size() << "\n";

    if (!path.empty()) {
        // Find the first node that is far enough to move towards
        AEVec2 next;
        bool found = false;
        for (AEVec2& node : path) {
            float dx = node.x - pos.x;
            float dy = node.y - pos.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 5.f) { // skip tiny nodes
                next = node;
                found = true;
                break;
            }
        }

        if (!found) {
            rb->velocity.x = 0.f;
            currentState = EnemyState::IDLE;
            //std::cout << "[Enemy] All path nodes too close\n";
            return;
        }

        // Direction
        AEVec2 dir = { next.x - pos.x, next.y - pos.y };
        float dist = sqrtf(dir.x * dir.x + dir.y * dir.y);
        if (dist > 0.01f) {
            dir.x /= dist;
            dir.y /= dist;

            // Horizontal movement
            rb->velocity.x = dir.x * base.stats.speed;

            // Jump if next node is higher
            float heightDiff = next.y - pos.y;
            if (heightDiff > 20.f) Jump(300.f);

            currentState = EnemyState::WALK;
            //std::cout << "[Enemy] Moving towards: (" << next.x << ", " << next.y << ")\n";
        }
        else {
            rb->velocity.x = 0.f;
            currentState = EnemyState::IDLE;
            //std::cout << "[Enemy] Reached node, stopping\n";
        }
    }
    else {
        rb->velocity.x = 0.f;
        currentState = EnemyState::IDLE;
        //std::cout << "[Enemy] Path empty, cannot follow player\n";
    }
}

void EnemyGameObject::UpdateAnimation() {
    if (!animator) return;

    Animation* anim = idleAnim;
    switch (currentState) {
    case EnemyState::IDLE: anim = idleAnim; break;
    case EnemyState::WALK: anim = walkAnim; break;
    case EnemyState::ATTACK: anim = attackAnim; break; // Can add attack later
    }

    animator->PlayAnimation(anim);
}

void EnemyGameObject::Render() {
    GameObject::Render();
}