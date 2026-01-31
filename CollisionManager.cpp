#include "CollisionManager.h"
#include "GameObjectManager.h"  
#include "SpriteManager.h"  

bool CheckBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size)
{
	// collision x-axis?
	bool collisionX = obj1Pos.x + obj2Size.x >= obj2Pos.x &&
		obj2Pos.x + obj2Size.x >= obj1Pos.x;
	// collision y-axis?
	bool collisionY = obj1Pos.y + obj1Size.y >= obj2Pos.y &&
		obj2Pos.y + obj2Size.y >= obj1Pos.y;
	// collision only if on both axes
	return collisionX && collisionY;
}

bool BoxToBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size)
{
	AEVec2 l1, r1, l2, r2;
	AEVec2Set(&l1, obj1Pos.x - obj1Size.x / 2.f, obj1Pos.y + obj1Size.y / 2.f);
	AEVec2Set(&r1, obj1Pos.x + obj1Size.x / 2.f, obj1Pos.y - obj1Size.y / 2.f);

	AEVec2Set(&l2, obj2Pos.x - obj2Size.x / 2.f, obj2Pos.y + obj2Size.y / 2.f);
	AEVec2Set(&r2, obj2Pos.x + obj2Size.x / 2.f, obj2Pos.y - obj2Size.y / 2.f);

	if (l1.x > r2.x || l2.x > r1.x)
		return false;
	
	if (r1.y > l2.y || r2.y > l1.y)
		return false;

	return true;
}



bool IsCursorOverRect(f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y)
{
	s32 screenMouseX, screenMouseY;
	AEInputGetCursorPosition(&screenMouseX, &screenMouseY);

	s32 mouseX = screenMouseX - AEGfxGetWindowWidth() / 2.f, mouseY = -(screenMouseY - AEGfxGetWindowHeight() / 2.f);

	f32 minX = pos_x - scale_x / 2.f;
	f32 maxX = pos_x + scale_x / 2.f;

	f32 minY = pos_y - scale_y / 2.f;
	f32 maxY = pos_y + scale_y / 2.f;

	return mouseX >= minX && mouseX <= maxX
		&& mouseY >= minY && mouseY <= maxY;

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
	AEVec2Set(&scale, owner->scale.x * size.x, owner->scale.y * size.y);
	return scale;
}

void Collider::AddToOvelappingVector(Collider* c)
{
	auto it = std::find(overlappingColliders.begin(), overlappingColliders.end(), c);
	// if not in list , call on collison enter & add to list
	if (it == overlappingColliders.end()) // cant find
	{
		if (OnCollisionEnter) OnCollisionEnter();
		overlappingColliders.push_back(c);
	}
	// if still in list, keep calling oncollisionover
	else {

		if (OnCollisionOver) OnCollisionOver();
	}
}

void Collider::RemoveFromOverlappingVector(Collider* c)
{

	auto it = std::find(overlappingColliders.begin(), overlappingColliders.end(), c);

	if (it == overlappingColliders.end()) return;

	// if still in list but no colliion, call oncollision exit and remove from list
	if (OnCollisionExit) OnCollisionExit();
	overlappingColliders.erase(it);
	//delete* it;
}

void Collider::Update()
{

	//bool isHover = IsCursorOverRect(
	//	owner->pos.x + center.x,
	//	owner->pos.y + center.y,
	//	owner->scale.x * size.x,
	//	owner->scale.y * size.y);
	//if (!isHovering && isHover)
	//{
	//	if (OnMouseEnter)OnMouseEnter();
	//}
	//else if (isHovering)
	//{
	//	if (isHover)
	//	{
	//		if (OnMouseOver) OnMouseOver();
	//	}
	//	else {
	//		if (OnMouseExit) OnMouseExit();
	//	}
	//}
	//isHovering = isHover;


	//if (isHovering && AEInputCheckTriggered(AEVK_LBUTTON) && !isInteracting)
	//{
	//	if (OnMouseDown) OnMouseDown();
	//	isInteracting = true;
	//}
	//else if (isInteracting)
	//{
	//	if (AEInputCheckCurr(AEVK_LBUTTON))
	//	{
	//		if (OnClick) OnClick();
	//	}
	//	else if (AEInputCheckReleased(AEVK_LBUTTON))
	//	{
	//		if (OnMouseUp) OnMouseUp();
	//		isInteracting = false;
	//	}
	//}

	//if (isTrigger) return;


}

void Collider::Render()
{
	if (!owner->showColliders) return;
	Sprite* s = new Sprite(
		owner->scale.x * size.x,
		owner->scale.y * size.y,
		owner->pos.x + center.x,
		owner->pos.y + center.y,
		owner->pos.z,
		0.f,
		0xFF000000
		);
	s->opacity = 0.5f;

	s->Render();
}
