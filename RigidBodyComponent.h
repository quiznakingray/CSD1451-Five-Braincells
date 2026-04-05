#ifndef RIGID_BODY_COMPONENT
#define RIGID_BODY_COMPONENT

//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
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

	AEVec2 velocity{};
	AEVec2 prevVel{};
	AEVec2 acceleration{};
	f32 mass{};
	f32 invMass{};
	f32 drag{};

	f32 maxImpulse{};
	f32 maxSpeed{};

	bool onCollider = false;

	bool hasGravity = true;
	f32 gravity = -400.f;
	//AEVec2 FreePosition;


	void Update() override;
};

#endif // !RIGID_BODY_COMPONENT
