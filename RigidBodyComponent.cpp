#include "RigidBodyComponent.h"
#include "PhysicsManager.h"
#include "GameObjectManager.h"

void RigidBody::Update()
{
	if (!this->owner->isActive ) return;
	PhysicsManager::UpdateRigidBody(this, AEFrameRateControllerGetFrameTime());

	std::vector<Collider*> colliders = this->owner->GetComponents<Collider>();
	for (Collider* pCol : colliders)
	{
		for (CollisionInfo& info : pCol->collisionInfos)
		{
			if (!info.other->owner->isActive) continue;
			PhysicsManager::HandleCollision(pCol, info.other);
		}
	}
}
