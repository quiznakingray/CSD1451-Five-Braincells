#include "SpriteManager.h"
#include "CollisionManager.h"

#include <algorithm>

void Sprite::Init()
{

	// set sprite uv
	f32 u0 = 0.0f, v0 = 0.0f;
	f32 u1 = 1.0f, v1 = 1.0f;


	if (spriteSheet.isSpriteSheet)
	{
		spriteSheet.currentFrame = 0;
		spriteSheet.maxFrames = spriteSheet.rows * spriteSheet.columns;
		spriteSheet.UVWidth = 1.f / spriteSheet.columns;
		spriteSheet.UVHeight = 1.f / spriteSheet.rows;

		u32 currentSpriteRow = spriteSheet.currentFrame / spriteSheet.columns;
		u32 currentSpriteCol = spriteSheet.currentFrame % spriteSheet.columns;

		u0 = spriteSheet.UVWidth * currentSpriteCol;
		v0 = spriteSheet.UVHeight * currentSpriteRow;

		u1 = u0 + spriteSheet.UVWidth;
		v1 = v0 + spriteSheet.UVHeight;
	}


	AEGfxMeshStart();
	// add tri for rects
	if (spriteShape == SPRITE_SHAPE::SHAPE_RECT)
	{
		AEGfxTriAdd(
			-0.5f, -0.5f, meshColor, u0, v1,
			0.5f, -0.5f, meshColor, u1, v1,
			-0.5f, 0.5f, meshColor, u0, v0);

		AEGfxTriAdd(
			0.5f, -0.5f, meshColor, u1, v1,
			0.5f, 0.5f, meshColor, u1, v0,
			-0.5f, 0.5f, meshColor, u0, v0);
	}
	else if (spriteShape == SPRITE_SHAPE::SHAPE_CIRCLE)
	{


		f32 slice = 40.0f;
		f32 theta = 360.0f / slice;

		for (int i = 1; i <= slice; i++)
		{
			f32 x1 = AECosDeg((i - 1) * theta);
			f32 y1 = AESinDeg((i - 1) * theta);

			f32 uCircle1 = (x1 + 1.0f) * 0.5f;
			f32 vCircle1 = 1.0f - ((y1 + 1.0f) * 0.5f);

			// Remap to sprite frame
			f32 uFinal1 = u0 + uCircle1 * (u1 - u0);
			f32 vFinal1 = v0 + vCircle1 * (v1 - v0);

			AEGfxVertexAdd(x1, y1, meshColor, uFinal1, vFinal1);

			// Center vertex
			f32 centerU = u0 + 0.5f * (u1 - u0);
			f32 centerV = v0 + 0.5f * (v1 - v0);

			AEGfxVertexAdd(0.0f, 0.0f, meshColor, centerU, centerV);

			f32 x2 = AECosDeg(i * theta);
			f32 y2 = AESinDeg(i * theta);

			f32 uCircle2 = (x2 + 1.0f) * 0.5f;
			f32 vCircle2 = 1.0f - ((y2 + 1.0f) * 0.5f);

			f32 uFinal2 = u0 + uCircle2 * (u1 - u0);
			f32 vFinal2 = v0 + vCircle2 * (v1 - v0);

			AEGfxVertexAdd(x2, y2, meshColor, uFinal2, vFinal2);
		}
	}
	mesh = AEGfxMeshEnd(); // set to ui->mesh

	if (!textureFileName.empty() && !texture)
	{
		texture = AEGfxTextureLoad(textureFileName.c_str());
	}
}

void Sprite::Update()
{
	//AEVec2Set(&pos, owner->pos.x, owner->pos.y);
	//pos.z = owner->pos.z;
	//AEVec2Set(&scale, owner->scale.x, owner->scale.y);
	//if (spriteSheet.isSpriteSheet) {
	//	spriteSheet.animTimer += (f32)AEFrameRateControllerGetFrameTime();
	//	if (spriteSheet.animTimer >= 1.0f / spriteSheet.animFPS)
	//	{
	//		spriteSheet.animTimer = 0.0f;
	//		if (spriteSheet.currentFrame >= spriteSheet.maxFrames - 1  && !spriteSheet.loopAnim) return;

	//		spriteSheet.currentFrame = ++spriteSheet.currentFrame % spriteSheet.maxFrames;
	//		UpdateFrame();
	//	}
	//}
}

void Sprite::Render()  {

	// calculate row and columns 
	f32 scaleX = spriteShape == SPRITE_SHAPE::SHAPE_CIRCLE ? owner->scale.x / 2  : owner->scale.x;
	f32 scaleY = spriteShape == SPRITE_SHAPE::SHAPE_CIRCLE ? owner->scale.x / 2 : owner->scale.y;
	AEMtx33 scaleMtx = { 0 };
	AEMtx33Scale(&scaleMtx, scaleX, scaleY);


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
	if (texture != nullptr) AEGfxTextureSet(texture, spriteSheet.isSpriteSheet ? spriteSheet.UVOffsetX : 0, spriteSheet.isSpriteSheet ? spriteSheet.UVOffsetY : 0);
	// Tell Alpha Engine to draw the mesh with the above settings.
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);


}

void Sprite::Free()
{
	if (mesh != nullptr) AEGfxMeshFree(mesh);
	if (texture != nullptr) {
		AEGfxTextureUnload(texture);
		texture = nullptr;
	}
}


//void RenderSprite(Sprite sprite, AEGfxVertexList* mesh)
//{
//	// Tell the engine to get ready to draw something with texture.
//	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
//
//	// Set the the color to multiply to white, so that the sprite can 
//	// display the full range of colors (default is black).
//	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
//
//	// Set the color to add to nothing, so that we don't alter the sprite's color
//	AEGfxSetColorToAdd(sprite.addColor.r, sprite.addColor.g, sprite.addColor.b, sprite.addColor.a);
//
//	// Set blend mode to AE_GFX_BM_BLEND
//	// This will allow transparency.
//	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
//	AEGfxSetTransparency(1.0f);
//
//	// Set the texture to pTex
//	AEGfxTextureSet(sprite.texture, 0, 0);
//
//	AEMtx33 scale = { 0 };
//	AEMtx33Scale(&scale, sprite.owner->scale.x, sprite.owner->scale.y);
//
//
//	AEMtx33 rotate = { 0 };
//	AEMtx33Rot(&rotate, sprite.owner->rotation);
//
//
//	AEMtx33 translate = { 0 };
//	AEMtx33Trans(&translate, sprite.owner->pos.x, sprite.owner->pos.y);
//
//
//	AEMtx33 transform = { 0 };
//	AEMtx33Concat(&transform, &rotate, &scale);
//	AEMtx33Concat(&transform, &translate, &transform);;
//
//
//	// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
//	AEGfxSetTransform(transform.m);
//	// the vertices of the mesh that we are about to choose to draw in the next line.
//
//	// Tell Alpha Engine to draw the mesh with the above settings.
//	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
//}

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

void Sprite::UpdateFrame()
{
	u32 currentSpriteRow = spriteSheet.currentFrame / spriteSheet.columns;
	u32 currentSpriteCol = spriteSheet.currentFrame % spriteSheet.columns;

	f32 u0 = spriteSheet.UVWidth * currentSpriteCol;
	f32 v0 = spriteSheet.UVHeight * currentSpriteRow;

	spriteSheet.UVOffsetX = u0;
	spriteSheet.UVOffsetY = v0;
}
