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
struct Collider : ComponentBase {
	
	COLLIDER_TYPE type;
	AEVec2 center{};
	AEVec2 size{};

	bool canCollide = true;
	bool canInteract = true;

	bool isTrigger = true;
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

	//Collision Function
	std::function<void(Collider *)> OnCollisionEnter;
	std::function<void(Collider *)> OnCollisionOver;
	std::function<void(Collider *)> OnCollisionExit;

	std::vector<Collider*> overlappingColliders{};

	//Collider() = default;
	Collider(COLLIDER_TYPE c_type = COLLIDER_TYPE::BOX_COLLIDER, f32 center_x = 0.f, f32 center_y = 0.f, f32 size_x = 1.f, f32 size_y = 1.f) {
		AEVec2Set(&center, center_x, center_y);
		AEVec2Set(&size, size_x, size_y);
		type = c_type;
	}

	AEVec3 GetPos();
	AEVec2 GetPos2D();
	AEVec2 GetScale();

	void AddToOvelappingVector(Collider* c);
	void RemoveFromOverlappingVector(Collider* c);



	void Update() override;
	void Render() override;
};


bool CheckBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size);

bool BoxToBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size);

bool IsCursorOverRect(f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y);

#endif // !COLLISION_MANAGER_H