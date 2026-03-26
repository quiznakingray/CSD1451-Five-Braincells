#include "PhysicsManager.h"
#include "MapManager.h"
#include "GameObjectManager.h"
#include <iostream>

void PhysicsManager::UpdateRigidBody(RigidBody* rb, f32 dt)
{
    if (!rb) return;

    rb->onCollider = false;
    // Apply gravity
    if (rb->hasGravity ) rb->velocity.y += rb->gravity * dt;

    //// Update velocity from acceleration
    //rb->velocity.x += rb->acceleration.x * dt;
    //rb->velocity.y += rb->acceleration.y * dt;

    // Update position from velocity
    rb->owner->pos.x += rb->velocity.x * dt;
    rb->owner->pos.y += rb->velocity.y * dt;
    //std::cout << "onCollider: " << rb->onCollider << '\n';
}

void PhysicsManager::HandleCollision(Collider* a, Collider* b)
{
    if (!a || !b) return;
    GameObject* A = a->owner;
    GameObject* B = b->owner;
    if (!A || !B) return;

    // skip player-crate resolution when grabbing
    CrateTile* crateA = dynamic_cast<CrateTile*>(A);
    CrateTile* crateB = dynamic_cast<CrateTile*>(B);
    Player* playerA = dynamic_cast<Player*>(A);
    Player* playerB = dynamic_cast<Player*>(B);

    if ((crateA && crateA->pushState && playerB) ||
        (crateB && crateB->pushState && playerA))
        return;

    RigidBody* ra = A->GetComponent<RigidBody>();
    RigidBody* rb = B->GetComponent<RigidBody>();

    if (a->isTrigger || b->isTrigger) return;

    float dx = a->GetPos2D().x - b->GetPos2D().x;
    float dy = a->GetPos2D().y - b->GetPos2D().y;

    float pxOverlap = (a->GetScale().x * 0.5f + b->GetScale().x * 0.5f) - fabs(dx);
    float pyOverlap = (a->GetScale().y * 0.5f + b->GetScale().y * 0.5f) - fabs(dy);

    if (pxOverlap <= 0 || pyOverlap <= 0) return;

    bool resolveX = pxOverlap < pyOverlap;

    auto move = [](GameObject* obj, float amountX, float amountY, RigidBody* r, bool zeroXForStatic = false) {
        obj->pos.x += amountX;
        obj->pos.y += amountY;
        if (!r) return;

        // Only zero vertical velocity when resolving Y overlap
        if (amountY != 0) {
            r->velocity.y = 0;
            if (amountY > 0) r->onCollider = true; // landed on floor
        }

        // Zero horizontal velocity only for static collisions
        if (zeroXForStatic && amountX != 0) {
            r->velocity.x = 0;
        }
        };

    if ((!ra || ra->type == RIGIDBODY_TYPE::STATIC) &&
        (!rb || rb->type == RIGIDBODY_TYPE::STATIC))
        return;

    if (ra && rb &&
        ra->type == RIGIDBODY_TYPE::DYNAMIC &&
        rb->type == RIGIDBODY_TYPE::DYNAMIC)
    {
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
            if (dy > 0) { move(A, 0, pyOverlap * aRatio, ra); move(B, 0, -pyOverlap * bRatio, rb); }
            else { move(A, 0, -pyOverlap * aRatio, ra); move(B, 0, pyOverlap * bRatio, rb); }
        }
    }
    else
    {
        RigidBody* dynamicRb = (ra && ra->type == RIGIDBODY_TYPE::DYNAMIC) ? ra : rb;
        if (!dynamicRb) return;
        GameObject* dynamicObj = dynamicRb->owner;
        if (!dynamicObj) return;

        if (!dynamicObj) return;

        if (resolveX)
        {
            if (dx > 0)
                dynamicObj->pos.x += pxOverlap;
            else
                dynamicObj->pos.x -= pxOverlap;

            dynamicRb->velocity.x = 0.f;
        }
        else
        {
            // Only snap if dynamic is falling onto static
            if (dynamicRb->velocity.y <= 0.f && dy > 0.f) // going down
            {
                // landed on top
                dynamicObj->pos.y += pyOverlap;
                dynamicRb->velocity.y = 0.f;
                dynamicRb->onCollider = true;
            }
            else
            {
                // hit ceiling
                dynamicObj->pos.y -= pyOverlap;
                dynamicRb->velocity.y = 0.f;
            }

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
