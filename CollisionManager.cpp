#include "CollisionManager.h"
#include "GameObjectManager.h"  
#include "SpriteManager.h"  

#include <iostream>
//bool CheckBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size)
//{
//	// collision x-axis?
//	bool collisionX = obj1Pos.x + obj2Size.x >= obj2Pos.x &&
//		obj2Pos.x + obj2Size.x >= obj1Pos.x;
//	// collision y-axis?
//	bool collisionY = obj1Pos.y + obj1Size.y >= obj2Pos.y &&
//		obj2Pos.y + obj2Size.y >= obj1Pos.y;
//	// collision only if on both axes
//	return collisionX && collisionY;
//}



int GetAllCollisionSides(AEVec2 aPos, AEVec2 bPos, AEVec2 aScale, AEVec2 bScale)
{
	int sides = COLLISION_SIDE::NONE;

	float aLeft = aPos.x - aScale.x * 0.5f;
	float aRight = aPos.x + aScale.x * 0.5f;
	float aTop = aPos.y + aScale.y * 0.5f;
	float aBottom = aPos.y - aScale.y * 0.5f;

	float bLeft = bPos.x - bScale.x * 0.5f;
	float bRight = bPos.x + bScale.x * 0.5f;
	float bTop = bPos.y + bScale.y * 0.5f;
	float bBottom = bPos.y - bScale.y * 0.5f;

	float overlapLeft = aRight - bLeft;   // A's right into B's left
	float overlapRight = bRight - aLeft;   // B's right into A's left
	float overlapTop = aTop - bBottom; // A's top into B's bottom
	float overlapBottom = bTop - aBottom; // B's top into A's bottom

	// Use a threshold to avoid floating point noise
	float threshold = 1.0f;

	if (overlapLeft > 0 && overlapLeft < aScale.x * 0.5f + threshold) sides |= COLLISION_SIDE::RIGHT;
	if (overlapRight > 0 && overlapRight < aScale.x * 0.5f + threshold) sides |= COLLISION_SIDE::LEFT;
	if (overlapTop > 0 && overlapTop < aScale.y * 0.5f + threshold) sides |= COLLISION_SIDE::TOP;
	if (overlapBottom > 0 && overlapBottom < aScale.y * 0.5f + threshold) sides |= COLLISION_SIDE::BOTTOM;

	return sides;
}

bool BoxToCircleCollision(AEVec2 boxPos, AEVec2 circlePos, AEVec2 boxScale, AEVec2 circleScale)
{
	float halfW = boxScale.x * 0.5f;
	float halfH = boxScale.y * 0.5f;

	// Find closest point to circle inside box
	float closestX = circlePos.x;
	float closestY = circlePos.y;

	if (circlePos.x < boxPos.x - halfW)
		closestX = boxPos.x - halfW;
	else if (circlePos.x > boxPos.x + halfW)
		closestX = boxPos.x + halfW;

	if (circlePos.y < boxPos.y - halfH)
		closestY = boxPos.y - halfH;
	else if (circlePos.y > boxPos.y + halfH)
		closestY = boxPos.y + halfH;

	float dx = circlePos.x - closestX;
	float dy = circlePos.y - closestY;

	float distanceSquared = dx * dx + dy * dy;

	float radius = circleScale.x * 0.5f; 

	return distanceSquared <= radius * radius;
}

bool CircleToCircleCollision(AEVec2 aPos, AEVec2 bPos, AEVec2 aScale, AEVec2 bScale)
{
	float dx = aPos.x - bPos.x;
	float dy = aPos.y - bPos.y;

	float distanceSq = dx * dx + dy * dy;
	float radiusSum = aScale.x + bScale.x;

	return distanceSq <= (radiusSum * radiusSum);
}

bool BoxToBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size)
{
	// Check X overlap
	if (obj1Pos.x + obj1Size.x * 0.5f < obj2Pos.x - obj2Size.x * 0.5f || obj1Pos.x - obj1Size.x * 0.5f > obj2Pos.x + obj2Size.x * 0.5f)
		return false;

	// Check Y overlap
	if (obj1Pos.y + obj1Size.y * 0.5f < obj2Pos.y - obj2Size.y * 0.5f || obj1Pos.y - obj1Size.y * 0.5f > obj2Pos.y + obj2Size.y * 0.5f)
		return false;

	return true;
}

int FlipCollisionSides(int sides)
{
	int flipped = COLLISION_SIDE::NONE;
	if (sides & COLLISION_SIDE::TOP)    flipped |= COLLISION_SIDE::BOTTOM;
	if (sides & COLLISION_SIDE::BOTTOM) flipped |= COLLISION_SIDE::TOP;
	if (sides & COLLISION_SIDE::LEFT)   flipped |= COLLISION_SIDE::RIGHT;
	if (sides & COLLISION_SIDE::RIGHT)  flipped |= COLLISION_SIDE::LEFT;
	return flipped;
}


bool IsPosInRect(AEVec2 pos, AEVec2 rectPos, AEVec2 rectScale)
{
	f32 minX = rectPos.x - rectScale.x / 2.f;
	f32 maxX = rectPos.x + rectScale.x / 2.f;

	f32 minY = rectPos.y - rectScale.y / 2.f;
	f32 maxY = rectPos.y + rectScale.y / 2.f;

	return pos.x >= minX && pos.x <= maxX
		&& pos.y >= minY && pos.y <= maxY;
}

bool IsCursorOverRect(f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y)
{
	s32 screenX, screenY;
	f32 mouseInWorldX, mouseInWorldY;
	AEInputGetCursorPosition(&screenX, &screenY);
	mouseInWorldX = screenX + AEGfxGetWinMinX();
	mouseInWorldY = -(screenY - AEGfxGetWinMaxY());
	f32 minX = pos_x - scale_x / 2.f;
	f32 maxX = pos_x + scale_x / 2.f;

	f32 minY = pos_y - scale_y / 2.f;
	f32 maxY = pos_y + scale_y / 2.f;
	//std::cout << "mouse:" << mouseInWorldX << "  " << mouseInWorldY << std::endl;
	return mouseInWorldX >= minX && mouseInWorldX <= maxX
		&& mouseInWorldY >= minY && mouseInWorldY <= maxY;

}


AEVec3 Collider::GetPos()
{
	AEVec3 position{};
	position.x = owner->pos.x + center.x;
	position.y = owner->pos.y + center.y;
	position.z = owner->pos.z;
	return position;
}
AEVec2 Collider::GetPos2D()
{
	AEVec2 position{};
	position.x = owner->pos.x + center.x;
	position.y = owner->pos.y + center.y;
	return position;
}

AEVec2 Collider::GetScale()
{
	AEVec2 scale{};
	f32 scaleY = owner->scale.y * size.y;
	AEVec2Set(&scale, owner->scale.x * size.x, owner->scale.y * size.y);
	return scale;
}


void Collider::AddToOvelappingVector(Collider* c, int inSides)
{
	auto it = std::find_if(collisionInfos.begin(), collisionInfos.end(),
		[c](const CollisionInfo& info) { return info.other == c; });

	if (it == collisionInfos.end())
	{
		collisionInfos.push_back({ c, inSides });
		if (isTrigger) { if (OnTriggerEnter) OnTriggerEnter(c, inSides); }
		else { if (OnCollisionEnter) OnCollisionEnter(c, inSides); }
	}
	else
	{
		it->sides = inSides;
		if (isTrigger) { if (OnTriggerOver) OnTriggerOver(c, inSides); }
		else { if (OnCollisionOver) OnCollisionOver(c, inSides); }
	}

	// Rebuild the combined sides bitmask across ALL current collisions
	sides = COLLISION_SIDE::NONE;
	for (const CollisionInfo& info : collisionInfos)
		sides |= info.sides;
}

void Collider::RemoveFromOverlappingVector(Collider* c)
{
	auto it = std::find_if(collisionInfos.begin(), collisionInfos.end(),
		[c](const CollisionInfo& info) { return info.other == c; });

	if (it == collisionInfos.end()) return;

	int lastSides = it->sides; // capture sides before removing

	if (isTrigger) { if (OnTriggerExit)   OnTriggerExit(c, lastSides); }
	else { if (OnCollisionExit) OnCollisionExit(c, lastSides); }

	collisionInfos.erase(it);

	// Rebuild combined sides bitmask now that this collider is removed
	sides = COLLISION_SIDE::NONE;
	for (const CollisionInfo& info : collisionInfos)
		sides |= info.sides;
}

void Collider::Update()
{


}

void Collider::Render()
{
	if (!owner->showColliders) return;

	Sprite s;
	s.owner = owner;
	s.meshColor = isTrigger ? 0xFFFFFF00 : 0xFFFF0000;
	s.opacity = 0.5f;
	s.spriteShape = (type == COLLIDER_TYPE::CIRCLE_COLLIDER)
		? SPRITE_SHAPE::SHAPE_CIRCLE
		: SPRITE_SHAPE::SHAPE_RECT;
	s.Init();
	s.Render();
	s.Free();

}

void Collider::Free()
{
	// Tell every collider we're overlapping with that we're gone
	for (CollisionInfo& info : collisionInfos)
	{
		if (!info.other) continue;

		// Remove this collider from the other's list without triggering callbacks
		auto& otherInfos = info.other->collisionInfos;
		auto it = std::find_if(otherInfos.begin(), otherInfos.end(),
			[this](const CollisionInfo& i) { return i.other == this; });

		if (it != otherInfos.end())
			otherInfos.erase(it);
	}

	collisionInfos.clear();
	sides = COLLISION_SIDE::NONE;

	OnTriggerEnter = nullptr;
	OnTriggerOver = nullptr;
	OnTriggerExit = nullptr;
	OnCollisionEnter = nullptr;
	OnCollisionOver = nullptr;
	OnCollisionExit = nullptr;
	OnClick = nullptr;
	OnMouseDown = nullptr;
	OnMouseUp = nullptr;
	OnMouseOver = nullptr;
	OnMouseEnter = nullptr;
	OnMouseExit = nullptr;

}
