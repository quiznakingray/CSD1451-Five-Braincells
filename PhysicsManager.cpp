#include "PhysicsManager.h"
#include "MapManager.h"
#include "GameObjectManager.h"
#include <iostream>

void PhysicsManager::UpdateRigidBody(RigidBody* rb, f64 dt)
{
    if (!rb) return;

    rb->onCollider = false;
    // Apply gravity
    if (rb->hasGravity ) rb->velocity.y += rb->gravity * static_cast<f32>(dt);

    constexpr float MAX_FALL_SPEED = -600.0f; // tune this, keep fabs < 80 * fps
    if (rb->velocity.y < MAX_FALL_SPEED)
        rb->velocity.y = MAX_FALL_SPEED;

    // Update position from velocity
    rb->owner->pos.x += rb->velocity.x * static_cast<f32>(dt);
    rb->owner->pos.y += rb->velocity.y * static_cast<f32>(dt);
    //std::cout << "onCollider: " << rb->onCollider << '\n';
}

void PhysicsManager::HandleCollision(Collider* a, Collider* b)
{
    if (!a || !b) return;
    GameObject* A = a->owner;
    GameObject* B = b->owner;
    if (!A || !B) return;

    CrateTile* crateA = dynamic_cast<CrateTile*>(A);
    CrateTile* crateB = dynamic_cast<CrateTile*>(B);
    Player* playerA = dynamic_cast<Player*>(A);
    Player* playerB = dynamic_cast<Player*>(B);
   
    if (playerA && playerB)
    {
        if (playerA->currentAction == PLAYER_ACTION::CRATEINTERACT ||
            playerB->currentAction == PLAYER_ACTION::CRATEINTERACT)
            return;
    }

    RigidBody* ra = A->GetComponent<RigidBody>();
    RigidBody* rb = B->GetComponent<RigidBody>();

    if (a->isTrigger || b->isTrigger) return;

    float dx = a->GetPos2D().x - b->GetPos2D().x;
    float dy = a->GetPos2D().y - b->GetPos2D().y;
    float pxOverlap = (a->GetScale().x * 0.5f + b->GetScale().x * 0.5f) - fabs(dx);
    float pyOverlap = (a->GetScale().y * 0.5f + b->GetScale().y * 0.5f) - fabs(dy);
    if (pxOverlap <= 0 || pyOverlap <= 0) return;

    auto move = [](GameObject* obj, float amountX, float amountY, RigidBody* r, bool zeroXForStatic = false) {
        obj->pos.x += amountX;
        obj->pos.y += amountY;
        if (!r) return;
        if (amountY != 0) {
            // only zero velocity if not jumping
            if (r->velocity.y <= 0.f)
                r->velocity.y = 0;
            if (amountY > 0) r->onCollider = true;
        }
        if (zeroXForStatic && amountX != 0) {
            r->velocity.x = 0;
        }
        };

    // Classify each body
    bool aIsStatic = !ra || ra->type == RIGIDBODY_TYPE::STATIC;
    bool bIsStatic = !rb || rb->type == RIGIDBODY_TYPE::STATIC;
    bool aIsKinematic = ra && ra->type == RIGIDBODY_TYPE::KINEMATIC;
    bool bIsKinematic = rb && rb->type == RIGIDBODY_TYPE::KINEMATIC;
    bool aIsDynamic = ra && ra->type == RIGIDBODY_TYPE::DYNAMIC;
    bool bIsDynamic = rb && rb->type == RIGIDBODY_TYPE::DYNAMIC;

    constexpr float GROUND_BIAS = 12.0f;
    constexpr float WALL_HIT_THRESHOLD = 8.0f;

    if (!aIsDynamic && !bIsDynamic)
    {
        // Kinematic vs Static � resolve the kinematic body
        if (aIsKinematic || bIsKinematic)
        {
            RigidBody* kinRb = aIsKinematic ? ra : rb;
            GameObject* kinObj = kinRb->owner;
            if (!kinObj) return;

            bool resolveX;
            if (kinRb->onCollider)
                resolveX = pxOverlap + GROUND_BIAS < pyOverlap;
            else
                resolveX = pxOverlap < pyOverlap;

            if (resolveX)
            {
                if (dx > 0) kinObj->pos.x += pxOverlap;
                else        kinObj->pos.x -= pxOverlap;
                kinRb->velocity.x = 0.f;
            }
            else
            {
                if (kinRb->velocity.y <= 0.f && dy > 0.f)
                {
                    kinObj->pos.y += pyOverlap;
                    kinRb->velocity.y = 0.f;
                    kinRb->onCollider = true;
                }
                else
                {
                    kinObj->pos.y -= pyOverlap;
                    kinRb->velocity.y = 0.f;
                }
            }
        }
        return;  // static vs static still skips
    }

    // Dynamic vs Dynamic � split resolution by mass (unchanged)
    if (aIsDynamic && bIsDynamic)
    {
        bool resolveX = pxOverlap < pyOverlap;
        float totalMass = ra->mass + rb->mass;
        float aRatio = rb->mass / totalMass;
        float bRatio = ra->mass / totalMass;
        if (resolveX)
        {
            if (dx > 0) { move(A, pxOverlap * aRatio, 0, ra); move(B, -pxOverlap * bRatio, 0, rb); }
            else { move(A, -pxOverlap * aRatio, 0, ra); move(B, pxOverlap * bRatio, 0, rb); }
        }
        else
        {
            // skip vertical resolution if either object is jumping upward
            if (ra->velocity.y > 0.f || rb->velocity.y > 0.f) return;

            if (dy > 0) { move(A, 0, pyOverlap * aRatio, ra); move(B, 0, -pyOverlap * bRatio, rb); }
            else { move(A, 0, -pyOverlap * aRatio, ra); move(B, 0, pyOverlap * bRatio, rb); }
        }
        return;
    }

    // One side is dynamic; the other is static or kinematic � push the dynamic body
    RigidBody* dynamicRb = aIsDynamic ? ra : rb;
    RigidBody* passiveRb = aIsDynamic ? rb : ra;  // static or kinematic
    GameObject* dynamicObj = dynamicRb->owner;
    if (!dynamicObj) return;


    bool resolveX;
    if (dynamicRb->onCollider)
        resolveX = pxOverlap + GROUND_BIAS < pyOverlap;
    else
        resolveX = pxOverlap < pyOverlap;

    if (resolveX)
    {
        // For kinematic bodies, factor in their lateral velocity so the
        // dynamic body isn't launched sideways by a slow-moving platform.
        float kinVelX = (passiveRb && passiveRb->type == RIGIDBODY_TYPE::KINEMATIC)
            ? passiveRb->velocity.x : 0.f;

        if (dx > 0) dynamicObj->pos.x += pxOverlap;
        else        dynamicObj->pos.x -= pxOverlap;

        if (pxOverlap > WALL_HIT_THRESHOLD)
        {
            // Match the kinematic's horizontal velocity instead of zeroing out,
            // so the dynamic body rides a moving wall rather than stopping dead.
            dynamicRb->velocity.x = kinVelX;
        }
    }
    else
    {
        if (dynamicRb->velocity.y <= 0.f && dy > 0.f)
        {
            // Landed on top � also inherit kinematic's vertical push (rising platform)
            dynamicObj->pos.y += pyOverlap;
            dynamicRb->velocity.y = 0.f;
            dynamicRb->onCollider = true;

            // If the platform is moving horizontally, carry the rider along
            if (passiveRb && passiveRb->type == RIGIDBODY_TYPE::KINEMATIC)
                dynamicObj->pos.x += passiveRb->velocity.x * static_cast<f32>(AEFrameRateControllerGetFrameTime());
        }
        else
        {
            // Hit ceiling
            dynamicObj->pos.y -= pyOverlap;
            dynamicRb->velocity.y = 0.f;
        }
    }
}

void PhysicsManager::ApplyImpulse(RigidBody* rb, float impulseX)
{
    if (!rb || rb->invMass == 0.0f) return;

    if (impulseX > rb->maxImpulse) impulseX = rb->maxImpulse;
    else if (impulseX < -rb->maxImpulse) impulseX = -rb->maxImpulse;

    rb->velocity.x += impulseX * rb->invMass;

    if (rb->velocity.x > rb->maxSpeed) rb->velocity.x = rb->maxSpeed;
    else if (rb->velocity.x < -rb->maxSpeed) rb->velocity.x = -rb->maxSpeed;
}
