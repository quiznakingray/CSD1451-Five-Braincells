#include "sprite.h"
#include <algorithm>
#include "utils_collision_check.h"
void RenderSprite(Sprite* sprite)
{
	// add tri for rects
	AEGfxTriAdd(
		-0.5f, -0.5f, sprite->color, 0.0f, 1.0f,
		0.5f, -0.5f, sprite->color, 1.0f, 1.0f,
		-0.5f, 0.5f, sprite->color, 0.0f, 0.0f);

	AEGfxTriAdd(
		0.5f, -0.5f, sprite->color, 1.0f, 1.0f,
		0.5f, 0.5f, sprite->color, 1.0f, 0.0f,
		-0.5f, 0.5f, sprite->color, 0.0f, 0.0f);

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

	AEGfxRenderMode renderMode = sprite->texture != nullptr ? AE_GFX_RM_TEXTURE : AE_GFX_RM_COLOR;
	AEGfxSetRenderMode(renderMode);

	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
	AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);

	if (sprite->texture != nullptr) AEGfxTextureSet(sprite->texture, 0, 0);

	AEGfxSetTransform(transform.m);

	AEGfxMeshDraw(sprite->mesh, AE_GFX_MDM_TRIANGLES);
}

void FreeSprite(Sprite*  sprite)
{
	if (sprite->mesh != nullptr)	AEGfxMeshFree(sprite->mesh);
	if (sprite->texture != nullptr)AEGfxTextureUnload(sprite->texture);
}

void AddSpriteToArray(std::vector<Sprite*> &spriteArr, Sprite* & s)
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
	for (Sprite *& s : spriteArr)
	{
		RenderSprite(s);
	}
}

void HandleSpriteInteraction(std::vector<Sprite*>& spriteArr)
{
	//std::vector<Sprite*>::iterator max_it = std::max_element(spriteArr.begin(), spriteArr.end(),
	//	[](Sprite*& a, Sprite*& b) {
	//		return a->pos.z < b->pos.z;
	//	});
	int highestInteractionZ = -1;
	for (Sprite* & s : spriteArr)
	{
		//s->isHovering = IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y);
		if (IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y) && highestInteractionZ < s->pos.z) {
			highestInteractionZ = s->pos.z;
		}
	}

	for (Sprite* & s : spriteArr)
	{
		s->isHovering = highestInteractionZ == s->pos.z && IsCursorOverRect(s->pos.x, s->pos.y, s->scale.x, s->scale.y);
	}
}
