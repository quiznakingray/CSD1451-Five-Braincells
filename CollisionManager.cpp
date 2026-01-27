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


void Collider::Update()
{

	bool isHover = IsCursorOverRect(
		owner->pos.x + center.x,
		owner->pos.y + center.y,
		owner->scale.x * size.x,
		owner->scale.y * size.y);
	if (!isHovering && isHover)
	{
		if (OnMouseEnter)OnMouseEnter();
	}
	else if (isHovering)
	{
		if (isHover)
		{
			if (OnMouseOver) OnMouseOver();
		}
		else {
			if (OnMouseExit) OnMouseExit();
		}
	}
	isHovering = isHover;


	if (isHovering && AEInputCheckTriggered(AEVK_LBUTTON) && !isInteracting)
	{
		if (OnMouseDown) OnMouseDown();
		isInteracting = true;
	}
	else if (isInteracting)
	{
		if (AEInputCheckCurr(AEVK_LBUTTON))
		{
			if (OnClick) OnClick();
		}
		else if (AEInputCheckReleased(AEVK_LBUTTON))
		{
			if (OnMouseUp) OnMouseUp();
			isInteracting = false;
		}
	}

	if (isTrigger) return;


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
		0xFFFF0000
		);
	s->opacity = 0.25f;

	s->Render();
}
