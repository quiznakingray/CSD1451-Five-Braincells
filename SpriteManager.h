#pragma once
#include <array>
#include <vector>
#include "AEEngine.h"
#include "Utils.h"
#include "CollisionManager.h"
#include "GameObjectManager.h"
#include "ComponentBase.h"

#define COLORSIZE 4

enum class SPRITE_DRAW_MODE {
	SIMPLE,
	TILED,
	SLICED
};

struct Sprite : ComponentBase{
	AEVec3 pos{};
	AEVec2 scale{};
	f32 rotation{};
	u32 color{};
	f32 opacity = 1.f;
	AEGfxVertexList* mesh = nullptr;
	AEGfxTexture* texture = nullptr;

	//std::array <f64, COLORSIZE > color{ };
	struct SpriteSheet {
		bool isSpriteSheet = false;
		s32 rows = 1, columns = 1;
		int currentFrame = 0;
	} spriteSheet;


	Sprite() : color(0xFF000000), rotation(0.f), texture(nullptr) {

	}
	Sprite(f32 scale_x, f32 scale_y, f32 pos_x, f32 pos_y, f32 pos_z = 0.f, f32 rot = 0.f, u32 c = 0xFF000000, AEGfxTexture* t = nullptr)
		: color(c), rotation(rot), texture(t)
	{
		AEVec2Set(&pos, pos_x, pos_y);
		pos.z = pos_z;
		AEVec2Set(&scale, scale_x, scale_y);

	}

	void Update() override;
	void Render() override;
	void Free() override;

};

void RenderSprite(Sprite sprite, AEGfxVertexList* mesh);

void HandleSpriteInteraction(std::vector<Sprite*>& spriteArr);

void UpdateSpriteArray(std::vector<Sprite*>& spriteArr);

void AddSpriteToArray(std::vector<Sprite*>& spriteArr, Sprite*& s);

void RenderSpriteArray(std::vector<Sprite*>& spriteArr);

void FreeSprite(Sprite* sprite);