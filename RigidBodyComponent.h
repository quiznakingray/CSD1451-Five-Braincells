#ifndef RIGID_BODY_COMPONENT
#define RIGID_BODY_COMPONENT

#include "AEEngine.h"
#include "ComponentBase.h"

enum class RIGIDBODY_TYPE
{
	STATIC,
	DYNAMIC,
	KINEMATIC
};
struct RigidBody : ComponentBase {

	RIGIDBODY_TYPE type = RIGIDBODY_TYPE::DYNAMIC;

	AEVec2 velocity;
	f32 mass;
	f32 drag;


	AEVec2 FreePosition;

	void Update() override;
};

#endif // !RIGID_BODY_COMPONENT
