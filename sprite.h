#ifndef SPRITE_H
#define SPRITE_h

#include <vector>
#include "AEEngine.h"

struct AEVec3 : AEVec2{
	f32 z;
};
struct Sprite {
	AEVec3 pos;
	AEVec2 scale;
	f32 rotation;
	u32 color;
	AEGfxVertexList* mesh = nullptr;
	AEGfxTexture* texture;
	
	bool hasCollision = false;
	bool isHovering = false;
	bool isInteracting = false;

	Sprite() = default;
	Sprite(f32 scale_x, f32 scale_y, f32 pos_x, f32 pos_y, f32 pos_z = 0.f, f32 rot = 0.f, u32 c = 0xFF000000, AEGfxTexture* t = nullptr)
		: color(c), rotation(rot), texture(t)
	{
		AEVec2Set(&pos, pos_x, pos_y);
		pos.z = pos_z;
		AEVec2Set(&scale, scale_x, scale_y);
	}
};

void RenderSprite(Sprite* sprite);
void FreeSprite(Sprite* sprite);

void AddSpriteToArray(std::vector<Sprite*> &spriteArr, Sprite* &s);
void RenderSpriteArray(std::vector<Sprite*> &spriteArr);
void HandleSpriteInteraction(std::vector<Sprite*>& spriteArr);

#endif