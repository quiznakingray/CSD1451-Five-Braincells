#include "SpriteManager.h"
#include "CollisionManager.h"

#include <algorithm>

void RenderSprite(Sprite* sprite) {

		// add tri for rects
	AEGfxTriAdd(
		-0.5f, -0.5f, sprite->color.r, 0.0f, 1.0f,
		0.5f, -0.5f, sprite->color.g, 1.0f, 1.0f,
		-0.5f, 0.5f, sprite->color.b, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, sprite->color.r, 1.0f, 1.0f,
		0.5f, 0.5f, sprite->color.g, 1.0f, 0.0f,
		-0.5f, 0.5f, sprite->color.b, 0.0f, 0.0f);

	sprite->mesh = AEGfxMeshEnd(); // set to ui->mesh

	AEMtx33 scale = { 0 };
	AEMtx33Scale(&scale, sprite->scale.x, sprite->scale.y);


	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, sprite->rotation);


	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, sprite->pos.x, sprite->pos.y);


	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);

	AEGfxSetTransform(transform.m);
	AEGfxSetColorToMultiply(0,
		0,
		0,
		0);
	// Set the color to add to nothing, so that we don't alter the sprite's color
	AEGfxSetColorToAdd((float)sprite->color.r,
		(float)sprite->color.g,
		(float)sprite->color.b,
		(float)sprite->color.a);
	AEGfxTextureSet(sprite->texture, 0, 0);
	// Tell Alpha Engine to draw the mesh with the above settings.
	AEGfxMeshDraw(sprite->mesh, AE_GFX_MDM_TRIANGLES);
}

void RenderSprite(Sprite sprite, AEGfxVertexList* mesh)
{
	// Tell the engine to get ready to draw something with texture.
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

	// Set the the color to multiply to white, so that the sprite can 
	// display the full range of colors (default is black).
	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

	// Set the color to add to nothing, so that we don't alter the sprite's color
	AEGfxSetColorToAdd(sprite.color.r, sprite.color.g, sprite.color.b, sprite.color.a);

	// Set blend mode to AE_GFX_BM_BLEND
	// This will allow transparency.
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	// Set the texture to pTex
	AEGfxTextureSet(sprite.texture, 0, 0);

	AEMtx33 scale = { 0 };
	AEMtx33Scale(&scale, sprite.scale.x, sprite.scale.y);


	AEMtx33 rotate = { 0 };
	AEMtx33Rot(&rotate, sprite.rotation);


	AEMtx33 translate = { 0 };
	AEMtx33Trans(&translate, sprite.pos.x, sprite.pos.y);


	AEMtx33 transform = { 0 };
	AEMtx33Concat(&transform, &rotate, &scale);
	AEMtx33Concat(&transform, &translate, &transform);;

	// Tell Alpha Engine to use the matrix in 'transform' to apply onto all
	// the vertices of the mesh that we are about to choose to draw in the next line.
	AEGfxSetTransform(transform.m);

	// Tell Alpha Engine to draw the mesh with the above settings.
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

void HandleSpriteInteraction(std::vector<Sprite*>& spriteArr)
{
	//std::vector<Sprite*>::iterator max_it = std::max_element(spriteArr.begin(), spriteArr.end(),
	//	[](Sprite*& a, Sprite*& b) {
	//		return a->pos.z < b->pos.z;
	//	});
	int highestInteractionZ = -1;
	for (Sprite*& s : spriteArr)
	{
		//s->isHovering = IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y);
		if (IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y) && highestInteractionZ < s->pos.z) {
			highestInteractionZ = static_cast<int>(s->pos.z);
		}
	}

	for (Sprite*& s : spriteArr)
	{
		if (!s->hasCollision) continue;

		bool isHover = highestInteractionZ == s->pos.z && IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y);
		if (!s->isHovering && isHover)
		{
			if (s->OnMouseEnter)s->OnMouseEnter();
		}
		else if (s->isHovering)
		{
			if (isHover)
			{
				if (s->OnMouseOver) s->OnMouseOver();
			}
			else {
				if (s->OnMouseExit) s->OnMouseExit();
			}
		}
		s->isHovering = isHover;


		if (s->isHovering && AEInputCheckTriggered(AEVK_LBUTTON) && !s->isInteracting)
		{
			if (s->OnMouseDown) s->OnMouseDown();
			s->isInteracting = true;
		}
		else if (s->isInteracting)
		{
			if (AEInputCheckCurr(AEVK_LBUTTON))
			{
				if (s->OnClick) s->OnClick();
			}
			else if (AEInputCheckReleased(AEVK_LBUTTON))
			{
				if (s->OnMouseUp) s->OnMouseUp();
				s->isInteracting = false;
			}
		}
	}
}

void AddSpriteToArray(std::vector<Sprite*>& spriteArr, Sprite*& s)
{
	spriteArr.push_back(s);

	// sort array
	std::sort(spriteArr.begin(), spriteArr.end(),
		[](Sprite*& a, Sprite*& b)
		{
			return a->pos.z < b->pos.z;
		}
	);


}

void RenderSpriteArray(std::vector<Sprite*>& spriteArr)
{
	for (Sprite*& s : spriteArr)
	{
		RenderSprite(s);
	}
}