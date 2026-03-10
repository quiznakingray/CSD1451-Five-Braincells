#include "EnemyGameObject.h"
#include "EnemyManager.h"
#include <iostream>
#include "EnemyMovement.h"

void EnemyGameObject::Init(EnemyType type, Tile* spawnTile) {
    GameObject::Init();
    InitEnemyBase(base, type);
    EnemyMovement::InitEnemyMovement(movement);

    if (!spawnTile) {
        std::cout << "[EnemyGameObject] No spawn tile provided!\n";
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
    Sprite* s = AddComponent(new Sprite());
    s->meshColor = (type == EnemyType::BASIC) ? 0xFF0000FF : 0xFFFF0000;
    s->textureFileName = "Assets/SpriteSheets/testRed4x6.png";
    s->spriteSheet = Sprite::SpriteSheet(6, 4);
    s->spriteSheet.isSpriteSheet = true;

    idleAnim = new Animation(s);
    walkAnim = new Animation(s);

    // Add Rigidbody first
    rb = AddComponent(new RigidBody());

    // Add Animator after Rigidbody
    animator = AddComponent(new Animator(idleAnim));

    // Collider
    Collider* c = AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

    base.isAlive = true;

    // Register enemy last
    //EnemyManager::RegisterEnemy(this);

    std::cout << "[EnemyGameObject] Initialized at (" << pos.x << ", " << pos.y << ")\n";
}

void EnemyGameObject::Update() {
    GameObject::Update();
    UpdateAnimation();
}

void EnemyGameObject::Patrol(f32 dt) {
    EnemyMovement::UpdateEnemyPatrol(this, dt);
    currentState = EnemyState::WALK;
}

void EnemyGameObject::FollowPlayer(AEVec2 playerPos, f32 dt) {
    // Call A* pathfinding to update velocity
    std::vector<AEVec2> path = EnemyMovement::FindPath(pos, playerPos);
    if (!path.empty()) {
        AEVec2 next = path.front();
        f32 dir = (next.x > pos.x) ? 1.f : -1.f;
        rb->velocity.x = dir * base.stats.speed;
        currentState = EnemyState::WALK;
    }
}

void EnemyGameObject::UpdateAnimation() {
    if (!animator) return;

    Animation* anim = nullptr;
    switch (currentState) {
    case EnemyState::IDLE: anim = idleAnim; break;
    case EnemyState::WALK: anim = walkAnim; break;
    case EnemyState::ATTACK: anim = walkAnim; break; // Can add attack later
    default: anim = idleAnim; break;
    }

    animator->PlayAnimation(anim);
}

void EnemyGameObject::Render() {
    GameObject::Render();
}