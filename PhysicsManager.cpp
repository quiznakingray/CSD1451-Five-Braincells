#include "PhysicsManager.h"
#include "GameObjectManager.h"


void PhysicsManager::UpdateRigidBody(RigidBody* rb, f32 dt)
{

    if (!rb) return;

    // Apply gravity
    if (rb->hasGravity && !rb->onCollider) rb->velocity.y += rb->gravity * dt;

    //// Update velocity from acceleration
    //rb->velocity.x += rb->acceleration.x * dt;
    //rb->velocity.y += rb->acceleration.y * dt;

    // Update position from velocity
    rb->owner->pos.x += rb->velocity.x * dt;
    rb->owner->pos.y += rb->velocity.y * dt;

    
}

void PhysicsManager::HandleCollision(Collider* a, Collider* b)
{
    if (!a || !b) return;

    GameObject* A = a->owner;
    GameObject* B = b->owner;
    if (!A || !B) return;

    RigidBody* ra = A->GetComponent<RigidBody>();
    RigidBody* rb = B->GetComponent<RigidBody>();

    // --- Trigger check ---
    if (a->isTrigger || b->isTrigger) {
        //if (a->OnCollisionOver) a->OnCollisionOver(b);
        //if (b->OnCollisionOver) b->OnCollisionOver(a);
        return; // triggers do not physically collide
    }

    // Compute overlap (AABB)
    float dx = A->pos.x - B->pos.x;
    float dy = A->pos.y - B->pos.y;

    float pxOverlap = (A->scale.x * 0.5f + B->scale.x * 0.5f) - fabs(dx);
    float pyOverlap = (A->scale.y * 0.5f + B->scale.y * 0.5f) - fabs(dy);

    if (pxOverlap <= 0 || pyOverlap <= 0) return; // no collision

    // --- Determine which axis to resolve first ---
    bool resolveX = pxOverlap < pyOverlap;

    auto move = [](GameObject* obj, float amountX, float amountY, RigidBody* r) {
        obj->pos.x += amountX;
        obj->pos.y += amountY;
        if (r) {
            if (amountX != 0) r->velocity.x = 0;
            if (amountY != 0) r->velocity.y = 0;
        }
        };

    // --- STATIC vs STATIC => nothing ---
    if ((!ra || ra->type == RIGIDBODY_TYPE::STATIC) &&
        (!rb || rb->type == RIGIDBODY_TYPE::STATIC))
        return;

    // --- DYNAMIC vs DYNAMIC => split overlap ---
    if (ra && rb &&
        ra->type == RIGIDBODY_TYPE::DYNAMIC &&
        rb->type == RIGIDBODY_TYPE::DYNAMIC)
    {
        float totalMass = ra->mass + rb->mass;
        float aRatio = rb->mass / totalMass;
        float bRatio = ra->mass / totalMass;

        if (resolveX)
        {
            if (dx > 0) {
                move(A, pxOverlap * aRatio, 0, ra);
                move(B, -pxOverlap * bRatio, 0, rb);
            }
            else {
                move(A, -pxOverlap * aRatio, 0, ra);
                move(B, pxOverlap * bRatio, 0, rb);
            }
        }
        else
        {
            if (dy > 0) {
                move(A, 0, pyOverlap * aRatio, ra);
                move(B, 0, -pyOverlap * bRatio, rb);
            }
            else {
                move(A, 0, -pyOverlap * aRatio, ra);
                move(B, 0, pyOverlap * bRatio, rb);
            }
        }
    }
    else // One dynamic, one static/kinematic => move dynamic only
    {
        RigidBody* dynamic = ra && ra->type == RIGIDBODY_TYPE::DYNAMIC ? ra : rb;
        GameObject* dynamicObj = dynamic->owner;
        GameObject* staticObj = (dynamicObj == A) ? B : A;

        if (!dynamicObj) return;

        float pushX = 0, pushY = 0;

        // Prioritize Y resolution
        if (resolveX)
        {
            pushX = dx > 0 ? pxOverlap : -pxOverlap;
        }
        else
        {
            // Only snap if dynamic is falling onto static
            if (dynamicObj->pos.y > staticObj->pos.y && dynamic->velocity.y <= 0.f)
            {
                dynamicObj->pos.y = staticObj->pos.y + (staticObj->scale.y * 0.5f) + (dynamicObj->scale.y * 0.5f);
                dynamic->velocity.y = 0.f;
                dynamic->onCollider = true;
            }
            else
            {
                dynamic->onCollider = false;
            }
        }

        // Move the dynamic object
        dynamicObj->pos.x += pushX;
        //dynamicObj->pos.y += pushY;
    }


    // --- Call collision callbacks ---
    //if (a->OnCollisionOver) a->OnCollisionOver(b);
    //if (b->OnCollisionOver) b->OnCollisionOver(a);
}
