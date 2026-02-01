#include "RigidBodyComponent.h"
#include "PhysicsManager.h"

void RigidBody::Update()
{
	PhysicsManager::UpdateRigidBody(this, AEFrameRateControllerGetFrameTime());


}
