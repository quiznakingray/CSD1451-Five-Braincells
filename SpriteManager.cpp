#include "SpriteManager.h"
#include "CollisionManager.h"

#include <algorithm>

void Sprite::Init()
{
	AEGfxMeshStart();

	//f32 row = spriteSheet.isSpriteSheet ?  spriteSheet.currentFrame / spriteSheet.rows : 1.f;
	//f32 column = spriteSheet.isSpriteSheet ? column * spriteSheet.currentFrame / spriteSheet.columns : 1.f;

	// add tri for rects
	AEGfxTriAdd(
		-0.5f, -0.5f, meshColor, 0.0f, 1.0f,
		0.5f, -0.5f, meshColor, 1.0f, 1.0f,
		-0.5f, 0.5f, meshColor, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, meshColor, 1.0f, 1.0f,
		0.5f, 0.5f, meshColor, 1.0f, 0.0f,
		-0.5f, 0.5f, meshColor, 0.0f, 0.0f);


	mesh = AEGfxMeshEnd(); // set to ui->mesh
}

void Sprite::Update()
{
	//AEVec2Set(&pos, owner->pos.x, owner->pos.y);
	//pos.z = owner->pos.z;
	//AEVec2Set(&scale, owner->scale.x, owner->scale.y);
}

void Sprite::Render()  {

	// calculate row and columns 

	if (spriteSheet.isSpriteSheet)
	{
		AEGfxMeshStart();

		//f32 row = spriteSheet.isSpriteSheet ?  spriteSheet.currentFrame / spriteSheet.rows : 1.f;
		//f32 column = spriteSheet.isSpriteSheet ? column * spriteSheet.currentFrame / spriteSheet.columns : 1.f;

		// add tri for rects
		AEGfxTriAdd(
			-0.5f, -0.5f, meshColor, 0.0f, 1.0f,
			0.5f, -0.5f, meshColor, 1.0f, 1.0f,
			-0.5f, 0.5f, meshColor, 0.0f, 0.0f);

		AEGfxTriAdd(
			0.5f, -0.5f, meshColor, 1.0f, 1.0f,
			0.5f, 0.5f, meshColor, 1.0f, 0.0f,
			-0.5f, 0.5f, meshColor, 0.0f, 0.0f);


		mesh = AEGfxMeshEnd(); // set to ui->mesh

	}

	AEMtx33 scaleMtx = { 0 };
	AEMtx33Scale(&scaleMtx, owner->scale.x, owner->scale.y);


	AEMtx33 rotateMtx = { 0 };
	AEMtx33Rot(&rotateMtx, owner->rotation);


	AEMtx33 translateMtx = { 0 };
	AEMtx33Trans(&translateMtx, owner->pos.x, owner->pos.y);


	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotateMtx, &scaleMtx);
	AEMtx33Concat(&transform, &translateMtx, &transform);

	AEGfxSetRenderMode(texture == nullptr  ? AE_GFX_RM_COLOR: AE_GFX_RM_TEXTURE);
	AEGfxSetColorToMultiply(multiplyColor.r, multiplyColor.g, multiplyColor.b, multiplyColor.a);
	AEGfxSetColorToAdd(addColor.r, addColor.g, addColor.b, addColor.a);
	AEGfxSetBlendMode(blendMode);
	AEGfxSetTransparency(opacity);
	AEGfxSetTransform(transform.m);
	if (texture != nullptr) AEGfxTextureSet(texture, 0, 0);
	// Tell Alpha Engine to draw the mesh with the above settings.
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);

}

void Sprite::Free()
{
	if (mesh != nullptr) AEGfxMeshFree(mesh);
	if (texture != nullptr)AEGfxTextureUnload(texture);
}


void RenderSprite(Sprite sprite, AEGfxVertexList* mesh)
{
	// Tell the engine to get ready to draw something with texture.
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	// Set the the color to multiply to white, so that the sprite can 
	// display the full range of colors (default is black).
	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

	// Set the color to add to nothing, so that we don't alter the sprite's color
	AEGfxSetColorToAdd(sprite.addColor.r, sprite.addColor.g, sprite.addColor.b, sprite.addColor.a);

	// Set blend mode to AE_GFX_BM_BLEND
	// This will allow transparency.
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	// Set the texture to pTex
	AEGfxTextureSet(sprite.texture, 0, 0);

	AEMtx33 scale = { 0 };
	AEMtx33Scale(&scale, sprite.owner->scale.x, sprite.owner->scale.y);


	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, sprite.owner->rotation);


	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, sprite.owner->pos.x, sprite.owner->pos.y);


	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);;


	// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
	AEGfxSetTransform(transform.m);
	// the vertices of the mesh that we are about to choose to draw in the next line.

	// Tell Alpha Engine to draw the mesh with the above settings.
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

void UpdateSpriteArray(std::vector<Sprite*>& spriteArr)
{
	HandleSpriteInteraction(spriteArr);
}

void HandleSpriteInteraction(std::vector<Sprite*>& spriteArr)
{

	//int highestInteractionZ = -1;
	//for (Sprite*& s : spriteArr)
	//{
	//	if (!s->isActive) continue;
	//	//s->isHovering = IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y);
	//	for (Collider *collider : s->colliders)
	//	{
	//		if (IsCursorOverRect(
	//			s->pos.x + collider->center.x,
	//			s->pos.y + collider->center.y, 
	//			s->scale.x * collider->size.x,
	//			s->scale.y * collider->size.y) 
	//			&& 
	//			highestInteractionZ < s->pos.z) {
	//			highestInteractionZ = static_cast<int>(s->pos.z);
	//		}

	//	}
	//}

	//for (Sprite*& s : spriteArr)
	//{
	//	if (!s->hasCollision || !s->isActive) continue;

	//	for (Collider* collider : s->colliders)
	//	{

	//		bool isHover = highestInteractionZ == s->pos.z && IsCursorOverRect(
	//			s->pos.x + collider->center.x,
	//			s->pos.y + collider->center.y,
	//			s->scale.x / collider->size.x,
	//			s->scale.y / collider->size.y)
	//			;
	//		if (!s->isHovering && isHover)
	//		{
	//			if (collider->OnMouseEnter)collider->OnMouseEnter();
	//		}
	//		else if (s->isHovering)
	//		{
	//			if (isHover)
	//			{
	//				if (collider->OnMouseOver) collider->OnMouseOver();
	//			}
	//			else {
	//				if (collider->OnMouseExit) collider->OnMouseExit();
	//			}
	//		}
	//		s->isHovering = isHover;


	//		if (s->isHovering && AEInputCheckTriggered(AEVK_LBUTTON) && !s->isInteracting)
	//		{
	//			if (collider->OnMouseDown) collider->OnMouseDown();
	//			s->isInteracting = true;
	//		}
	//		else if (s->isInteracting)
	//		{
	//			if (AEInputCheckCurr(AEVK_LBUTTON))
	//			{
	//				if (collider->OnClick) collider->OnClick();
	//			}
	//			else if (AEInputCheckReleased(AEVK_LBUTTON))
	//			{
	//				if (collider->OnMouseUp) collider->OnMouseUp();
	//				s->isInteracting = false;
	//			}
	//		}
	//	}
	//	//if(isHover && s->blockCollision) 

	//}
}

void AddSpriteToArray(std::vector<Sprite*>& spriteArr, Sprite*& s)
{
	spriteArr.push_back(s);

	// sort array
	std::sort(spriteArr.begin(), spriteArr.end(),
		[](Sprite*& a, Sprite*& b)
		{
			return a->owner->pos.z < b->owner->pos.z;
		}
	);


}

void RenderSpriteArray(std::vector<Sprite*>& spriteArr)
{
	for (Sprite*& s : spriteArr)
	{
		//RenderSprite(s);
		s->Render();
	}
}

void FreeSprite(Sprite* sprite)
{
	if (sprite->mesh != nullptr)	AEGfxMeshFree(sprite->mesh);
	if (sprite->texture != nullptr)AEGfxTextureUnload(sprite->texture);
}
