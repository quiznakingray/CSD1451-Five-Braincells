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
    delete attackingAnim; attackingAnim = nullptr; 

    FreeGameObjects(healthBarObjects);
    for (GameObject* go : healthBarObjects)
        delete go;
    healthBarObjects.clear();
    healthBarBG = nullptr;
    healthBarFG = nullptr;
}

void EnemyGameObject::Init(EnemyType type, Tile* spawnTile) {
    InitEnemyBase(base, type);
    EnemyMovement::InitEnemyMovement(movement);

    if (!spawnTile) return;

    // Base position
    AEVec2Set(&pos, spawnTile->pos.x + 50, spawnTile->pos.y);
    pos.z = 1.f;

    // Base scale (regular enemy)
    float scaleFactor = 1.f;

    // Make mini-bosses bigger
    if (type == EnemyType::MINI_BOSS_MELEE || type == EnemyType::MINI_BOSS_RANGED) {
        scaleFactor = 2.f; // 2x bigger, can adjust as needed
    }

    AEVec2Set(&scale, MapManager::tileSize * scaleFactor, MapManager::tileSize * scaleFactor);

    // Patrol area (optional, scale may influence it)
    base.patrolStart = { spawnTile->pos.x - 100.f * scaleFactor, spawnTile->pos.y - 100.f * scaleFactor };
    base.patrolEnd = { spawnTile->pos.x + 150.f * scaleFactor, spawnTile->pos.y - 100.f * scaleFactor };

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

    Sprite* attacking = new Sprite();
    attacking->meshColor = (type == EnemyType::BASIC_RANGED) ? 0xFF0000FF : 0xFFFF0000;
    attacking->textureFileName = "Assets/SpriteSheets/Enemy_Basic_Melee_Attacking.png";
    attacking->spriteSheet = Sprite::SpriteSheet(2, 7);
    attacking->spriteSheet.isSpriteSheet = true;

    patrolAnim = new Animation(s);
    patrolAnim->loopAnimation = true;
    patrolAnim->animationFPS = 10.f;

    chaseAnim = new Animation(walk);
    chaseAnim->loopAnimation = true;
    chaseAnim->animationFPS = 10.f;

    attackAnim = new Animation(attack);
    attackAnim->loopAnimation = true;
    attackAnim->animationFPS = 10.f;

    attackingAnim = new Animation(attacking);
    attackingAnim->loopAnimation = true;
    attackingAnim->animationFPS = 10.f;

    rb = AddComponent(new RigidBody());
    rb->type = RIGIDBODY_TYPE::DYNAMIC;
    rb->mass = 10;

    animator = AddComponent(new Animator(patrolAnim));
    AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

    healthText = AddComponent(new Text());
    healthText->SetText(std::to_string(base.stats.health));
    healthText->center.y = 100.f;

    base.isAlive = true;
    InitHealthBar();
    GameObject::Init();
}

void EnemyGameObject::Update() {
    float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
	isActive = base.isAlive;

    // If no players exist, just update animation/projectiles
    if (!EnemyManager::GetInstance().player1 && !EnemyManager::GetInstance().player2) {
        UpdateAnimation();
        GameObject::Update();
        //if (base.projectile) base.projectile->Update(); // Update arrow
        return;
    }

    healthText->SetText(std::to_string(base.stats.health));
    // Get position of the closest player
    AEVec3 playerPos = EnemyManager::GetInstance().GetClosestPlayerPos(pos);

    // Determine which player is closest for attack
    Player* targetPlayer = nullptr;
    float distToP1 = FLT_MAX, distToP2 = FLT_MAX;
    if (EnemyManager::GetInstance().player1) {
        float dx1 = EnemyManager::GetInstance().player1->pos.x - pos.x;
        float dy1 = EnemyManager::GetInstance().player1->pos.y - pos.y;
        distToP1 = dx1 * dx1 + dy1 * dy1;
    }
    if (EnemyManager::GetInstance().player2) {
        float dx2 = EnemyManager::GetInstance().player2->pos.x - pos.x;
        float dy2 = EnemyManager::GetInstance().player2->pos.y - pos.y;
        distToP2 = dx2 * dx2 + dy2 * dy2;
    }

    if (distToP1 <= distToP2)
        targetPlayer = EnemyManager::GetInstance().player1;
    else
        targetPlayer = EnemyManager::GetInstance().player2;

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
        EnemyAttackPlayer(base, *targetPlayer, pos, dt);
    }
    else if (distSq < detectionRangeSq && base.canMove) {
        if (isMelee) FollowPlayer(playerPos);
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
        if (base.canMove) Patrol();
    }

    //damage number 

    for (auto it = hurtTexts.begin(); it != hurtTexts.end();)
    {
        //float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
        it->second += dt;

        float offsetY = 80.f * it->second;
        it->first->center = { it->first->center.x, offsetY };
		it->first->SetColor({ 1.f, 0.f, 0.f, 1.f - (it->second / damageTextDuration) }); // fade out

        if (it->second >= damageTextDuration)
        {
            RemoveComponent(it->first); // nulled out
            it = hurtTexts.erase(it);
        }
        else ++it;
    }

	// sprite facing direction
	patrolAnim->sprite->size.x = fabs(patrolAnim->sprite->size.x) * (movement.movingRight? -1 : 1);
	chaseAnim->sprite->size.x = fabs(chaseAnim->sprite->size.x) * (movement.movingRight? -1 : 1);
	attackAnim->sprite->size.x = fabs(attackAnim->sprite->size.x) * (movement.movingRight? -1 : 1);
	attackingAnim->sprite->size.x = fabs(attackingAnim->sprite->size.x) * (movement.movingRight? -1 : 1);
    UpdateAnimation();
    GameObject::Update();

    if (isOnCamera)
        HandleWalkSFX(dt);
}

void EnemyGameObject::Patrol() {
    if (!base.canMove) return;
    EnemyMovement::UpdateEnemyPatrol(this);
    base.currentState = EnemyState::PATROL;
}

void EnemyGameObject::FollowPlayer(AEVec3 playerPos) {
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

	//movement.movingRight = (dx > 0);
}

void EnemyGameObject::UpdateAnimation() {
    if (!animator) return;
    Animation* anim = patrolAnim;

    switch (base.currentState)
    {
        case EnemyState::PATROL:
            anim = patrolAnim;
		    break;
        case EnemyState::CHASE:
            anim = chaseAnim;
            break;
        case EnemyState::ATTACK:
            anim = attackAnim;
		    break;
        case EnemyState::ATTACKING:
			anim = attackingAnim;
            break;

    }
	animator->PlayAnimation(anim);
    UpdateHealthBar();
}

void EnemyGameObject::InitHealthBar()
{
    float barW = scale.x * 0.8f;
    float barH = 10.f;
    float offsetY = scale.y / 2.f + 20.f; // float above enemy

    // Background (dark red)
    healthBarBG = new GameObject(barW, barH, pos.x, pos.y + offsetY, pos.z + 0.1f, 0, false);
    healthBarBG->AddComponent(new Sprite())->meshColor = 0xFF440000;

    // Foreground (bright green)
    healthBarFG = new GameObject(barW, barH, pos.x, pos.y + offsetY, pos.z + 0.2f, 0, false);
    healthBarFG->AddComponent(new Sprite())->meshColor = 0xFF00FF00;

    healthBarObjects.push_back(healthBarBG);
    healthBarObjects.push_back(healthBarFG);
    InitGameObjects(healthBarObjects);
} 
void EnemyGameObject::UpdateHealthBar()
{
    if (!healthBarBG || !healthBarFG) return;

    float offsetY = scale.y / 2.f + 20.f;
    float barW = scale.x * 0.8f;
    float ratio = (float)base.stats.health / (float)base.stats.maxHealth;
    ratio = ratio < 0.f ? 0.f : ratio > 1.f ? 1.f : ratio;

    // Follow enemy position
    healthBarBG->pos.x = pos.x;
    healthBarBG->pos.y = pos.y + offsetY;

    // Shrink foreground based on health ratio
    // Anchor left: offset x so it shrinks from right
    healthBarFG->pos.x = pos.x - (barW * (1.f - ratio)) / 2.f;
    healthBarFG->pos.y = pos.y + offsetY;
    healthBarFG->scale.x = barW * ratio;

    // Hide when full health, show when damaged
    healthBarBG->isActive = (base.stats.health <= base.stats.maxHealth);
    healthBarFG->isActive = (base.stats.health <= base.stats.maxHealth);

    // Color shifts green -> yellow -> red as health drops
    if (ratio > 0.5f) healthBarFG->GetComponent<Sprite>()->meshColor = 0xFF00FF00;
    else if (ratio > 0.25f) healthBarFG->GetComponent<Sprite>()->meshColor = 0xFF00FFFF;
    else                    healthBarFG->GetComponent<Sprite>()->meshColor = 0xFF0000FF;

    UpdateGameObjects(healthBarObjects);
}
void EnemyGameObject::Render() {
    if (!isActive) return;
    if (base.projectile) base.projectile->Render();
    RenderGameObjects(healthBarObjects);
    GameObject::Render();
}

void EnemyGameObject::Free()
{
    if (base.projectile)
    {
        base.projectile->Free();
        delete base.projectile;
		base.projectile = nullptr;
    }
	GameObject::Free();
}

void EnemyGameObject::HandleWalkSFX(float dt)
{
    // Only play if moving on X axis
    if (fabs(rb->velocity.x) > 0.1f)
    {
        walkSfxTimer -= dt;
        if (walkSfxTimer <= 0.f)
        {
            AudioManager::GetInstance().PlaySFX("wheels");
            walkSfxTimer = walkSfxInterval;
        }
    }
}