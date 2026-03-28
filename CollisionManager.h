#ifndef COLLISION_MANAGER_H
#define COLLISION_MANAGER_H

#include <functional>
#include <vector>
#include "AEEngine.h"
#include "Utils.h"
#include "ComponentBase.h"


enum class COLLIDER_TYPE {
	BOX_COLLIDER,
	CIRCLE_COLLIDER
};

enum COLLISION_SIDE {
	NONE = 0,
	TOP = 1 << 0,  // 0001
	BOTTOM = 1 << 1,  // 0010
	LEFT = 1 << 2,  // 0100
	RIGHT = 1 << 3   // 1000
};

// forward declaration
struct Collider;

struct CollisionInfo {
	Collider* other = nullptr;
	int sides = COLLISION_SIDE::NONE;
};

struct Collider : ComponentBase {
	COLLIDER_TYPE type;
	int sides = COLLISION_SIDE::NONE;       // bitmask of ALL
	AEVec2 center{};
	AEVec2 size{};

	bool canCollide = true;
	bool canInteract = true;

	bool isTrigger = false;
	bool isHovering = false;
	bool isInteracting = false;

	//Clicking Functions
	std::function<void()> OnClick; // When mouse is click & holding sprite
	std::function<void()> OnMouseDown; // Triggered once when sprite is clicked
	std::function<void()> OnMouseUp; // Triggered once when mouse button on sprite is released

	//Hovering Functions
	std::function<void()> OnMouseEnter;
	std::function<void()> OnMouseOver;
	std::function<void()> OnMouseExit;

	// Collision Functions — now pass sides bitmask as second param
	std::function<void(Collider*, int sides)> OnCollisionEnter;
	std::function<void(Collider*, int sides)> OnCollisionOver;
	std::function<void(Collider*, int sides)> OnCollisionExit;
	std::function<void(Collider*, int sides)> OnTriggerEnter;
	std::function<void(Collider*, int sides)> OnTriggerOver;
	std::function<void(Collider*, int sides)> OnTriggerExit;

	std::vector<CollisionInfo> collisionInfos{};

	//Collider() = default;
	Collider(COLLIDER_TYPE c_type = COLLIDER_TYPE::BOX_COLLIDER, f32 center_x = 0.f, f32 center_y = 0.f, f32 size_x = 1.f, f32 size_y = 1.f) {
		AEVec2Set(&center, center_x, center_y);
		f32 sizeY = type == COLLIDER_TYPE::CIRCLE_COLLIDER ? size_x : size_y;
		AEVec2Set(&size, size_x, sizeY);
		type = c_type;
	}

	AEVec3 GetPos();
	AEVec2 GetPos2D();
	AEVec2 GetScale();

	// Updated to accept and store sides
	void AddToOvelappingVector(Collider* c, int sides);
	void RemoveFromOverlappingVector(Collider* c);
	// Helper to check if a specific side is active
	bool IsCollidingOn(COLLISION_SIDE side) const { return (sides & side) != 0; }
	// Helper to get sides for a specific collider
	int GetSidesForCollider(Collider* c) const
	{
		for (const CollisionInfo& info : collisionInfos)
			if (info.other == c) return info.sides;
		return COLLISION_SIDE::NONE;
	}


	void Update() override;
	void Render() override;
	void Free() override;
};


//bool CheckBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size);
int  GetAllCollisionSides(AEVec2 aPos, AEVec2 bPos, AEVec2 aScale, AEVec2 bScale);
bool BoxToCircleCollision(AEVec2 boxPos, AEVec2 circlePos, AEVec2 boxScale, AEVec2 circleScale);
bool CircleToCircleCollision(AEVec2 aPos, AEVec2 bPos, AEVec2 aScale, AEVec2 bScale);
bool BoxToBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size);
int FlipCollisionSides(int sides); // flips sides to get B's perspective from A's
bool IsPosInRect(AEVec2 pos, AEVec2 rectPos, AEVec2 rectScale);
bool IsCursorOverRect(f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y);

#endif // !COLLISION_MANAGER_H