#pragma once
#include <array>
#include <vector>
#include <string>
#include "AEEngine.h"
#include "Utils.h"
#include "CollisionManager.h"
#include "GameObjectManager.h"
#include "ComponentBase.h"

#define COLORSIZE 4

enum class SPRITE_SHAPE {
	SHAPE_RECT,
	SHAPE_CIRCLE,
};

struct Color { // range form 0 to 1
	f32 r{};
	f32 g{};
	f32 b{};
	f32 a{};

	Color(f32 red = 1.f, f32 green = 1.f, f32 blue = 1.f, f32 alpha = 1.f)
		:r(red), g(green), b(blue), a(alpha) {
	}

};

using Color = struct Color;

struct Sprite : ComponentBase{
	//AEVec3 pos{};
	//AEVec2 scale{};
	//f32 rotation{};
	u32 meshColor{};
	Color multiplyColor = Color(1.f, 1.f, 1.f, 1.f);
	Color addColor = Color(0.f, 0.f, 0.f, 0.f);
	AEGfxBlendMode blendMode = AE_GFX_BM_BLEND;
	f32 opacity = 1.f;
	AEGfxVertexList* mesh = nullptr;
	AEGfxTexture* texture = nullptr;
	std::string textureFileName{};
	SPRITE_SHAPE spriteShape = SPRITE_SHAPE::SHAPE_RECT;
	//std::array <f64, COLORSIZE > color{ };
	struct SpriteSheet {
		bool isSpriteSheet = false;
		s32 rows = 1, columns = 1;
		int currentFrame = 0, maxFrames = 0;
		f32 UVWidth = 1.0f, UVHeight = 1.0f;
		f32 UVOffsetX = 0.0f, UVOffsetY = 0.0f;
		//f32 animFPS = 1.f;
		//bool loopAnim = true;
		//f32 animTimer = 0.0f;
		SpriteSheet(s32 r, s32 col, int currFrame = 0) : rows(r), columns(col), currentFrame(currFrame) {

		}

	} spriteSheet;


	Sprite() : meshColor(0x00000000), texture(nullptr) , spriteSheet(1, 1){

	}
	Sprite(
		f32 scale_x, f32 scale_y, 
		f32 pos_x, f32 pos_y, f32 pos_z = 0.f, 
		f32 rot = 0.f, 
		u32 c = 0xFF000000,
		AEGfxTexture* t = nullptr,
		SPRITE_SHAPE shape = SPRITE_SHAPE::SHAPE_RECT)
		: meshColor(c),texture(t), spriteSheet(1, 1), spriteShape(shape)
	{
		//AEVec2Set(&pos, pos_x, pos_y);
		//pos.z = pos_z;
		//AEVec2Set(&scale, scale_x, scale_y);

	}

	void UpdateFrame();
	void Init() override;
	void Update() override;
	void Render() override;
	void Free() override;


};

//void RenderSprite(Sprite sprite, AEGfxVertexList* mesh);

void HandleSpriteInteraction(std::vector<Sprite*>& spriteArr);

void UpdateSpriteArray(std::vector<Sprite*>& spriteArr);

void AddSpriteToArray(std::vector<Sprite*>& spriteArr, Sprite*& s);

void RenderSpriteArray(std::vector<Sprite*>& spriteArr);

void FreeSprite(Sprite* sprite);