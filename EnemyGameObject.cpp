#include "EnemyGameObject.h"
#include "EnemyManager.h"
#include <iostream>
#include "EnemyMovement.h"

void EnemyGameObject::Init(EnemyType type, Tile* spawnTile) {
    GameObject::Init();
    InitEnemyBase(base, type);
    EnemyMovement::InitEnemyMovement(movement);

    if (!spawnTile) return;

    // Position
    AEVec2Set(&pos, spawnTile->pos.x, spawnTile->pos.y);
    pos.z = 1.f;

    // Scale
    AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);

    // Patrol points around spawn
    base.patrolStart = { pos.x - 100.f, pos.y };
    base.patrolEnd = { pos.x + 150.f, pos.y };

    // Sprite setup
    Sprite* s = AddComponent(new Sprite());
    s->meshColor = (type == EnemyType::BASIC) ? 0xFF0000FF : 0xFFFF0000;
    s->textureFileName = "Assets/SpriteSheets/testRed4x6.png";
    s->spriteSheet = Sprite::SpriteSheet(6, 4);
    s->spriteSheet.isSpriteSheet = true;

    idleAnim = new Animation(s);
    walkAnim = new Animation(s);

    animator = AddComponent(new Animator(idleAnim));

    // Collider
    Collider* c = AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));
    rb = AddComponent(new RigidBody());

    base.isAlive = true;

    EnemyManager::RegisterEnemy(this);
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