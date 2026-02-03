#include "RigidBodyComponent.h"
#include "PhysicsManager.h"
#include "GameObjectManager.h"

void RigidBody::Update()
{
	if (!this->owner->isActive ||!this->owner->isOnCamera) return;
	PhysicsManager::UpdateRigidBody(this, AEFrameRateControllerGetFrameTime());

}
