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
	// Check X overlap
	if (obj1Pos.x + obj1Size.x * 0.5f < obj2Pos.x - obj2Size.x * 0.5f || obj1Pos.x - obj1Size.x * 0.5f > obj2Pos.x + obj2Size.x * 0.5f)
		return false;

	// Check Y overlap
	if (obj1Pos.y + obj1Size.y * 0.5f < obj2Pos.y - obj2Size.y * 0.5f || obj1Pos.y - obj1Size.y * 0.5f > obj2Pos.y + obj2Size.y * 0.5f)
		return false;

	return true;
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
		if (OnCollisionEnter) OnCollisionEnter(c);
		overlappingColliders.push_back(c);
	}
	// if still in list, keep calling oncollisionover
	else {

		if (OnCollisionOver) OnCollisionOver(c);
	}
}

void Collider::RemoveFromOverlappingVector(Collider* c)
{

	auto it = std::find(overlappingColliders.begin(), overlappingColliders.end(), c);

	if (it == overlappingColliders.end()) return;

	// if still in list but no colliion, call oncollision exit and remove from list
	if (OnCollisionExit) OnCollisionExit(c);
	overlappingColliders.erase(it);
	//delete* it;
}

void Collider::Update()
{


}

void Collider::Render()
{
	if (!owner->showColliders) return;

	GameObject* c = new GameObject(	
			owner->scale.x * size.x,
			owner->scale.y * size.y,
			owner->pos.x + center.x,
			owner->pos.y + center.y,
			owner->pos.z);

	Sprite* s = c->AddComponent(
		new Sprite()
	);

	s->meshColor = 0xFFFF0000;
	s->opacity = 0.5f;

	c->Render();

	//Sprite* s = new Sprite(
	//	owner->scale.x * size.x,
	//	owner->scale.y * size.y,
	//	owner->pos.x + center.x,
	//	owner->pos.y + center.y,
	//	owner->pos.z,
	//	0.f,
	//	0xFF000000
	//	);
	//s->opacity = 0.5f;

	//s->Render();
}
