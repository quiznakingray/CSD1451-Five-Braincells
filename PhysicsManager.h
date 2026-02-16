#ifndef PHYSICS_MANAGER
#define PHYSICS_MANAGER
#include "AEEngine.h"
#include "RigidBodyComponent.h"
#include "CollisionManager.h"

struct PhysicsManager {

	static void UpdateRigidBody(RigidBody* rb, f32 dt);
	
	static void HandleCollision(Collider* a, Collider* b);
};
#endif // !PHYSICS_MANAGER

