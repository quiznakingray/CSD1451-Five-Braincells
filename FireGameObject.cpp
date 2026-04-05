//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "FireGameObject.h"
#include "SpriteManager.h"
#include "AnimatorComponent.h"
#include "AEEngine.h"
#include "MapManager.h"

static std::vector<FireGameObject*> firePool;
void FireGameObject::Init()
{
	float randScale = MapManager::GetInstance().tileSize / 2.0f + AERandFloat() * MapManager::GetInstance().tileSize;
	scale = {randScale, randScale};
	Sprite* s = new Sprite();
	s->textureFileName = "Assets/SpriteSheets/Fire.png";
	s->spriteSheet = Sprite::SpriteSheet(1, 7);
	s->spriteSheet.isSpriteSheet = true;
	s->opacity = 0.75f;
	s->offset = { 0.f, randScale * 0.5f - MapManager::tileSize * 0.5f };

	fireAnim = new Animation(s);
	fireAnim->loopAnimation = true;
	fireAnim->animationFPS = 10.f;

	AddComponent(new Animator(fireAnim));

	GameObject::Init();
}

void FireGameObject::Free()
{
	GameObject::Free();
	if (fireAnim) {
		fireAnim->Free();
		delete fireAnim;
		fireAnim = nullptr;
	}
}



void SpawnFire(std::vector<GameObject*>& gos)
{
	auto fires = MapManager::GetInstance().GetTilesWithID(TILE_ID::EMPTYWITHFIRE);

	for (Tile* t : fires)
	{

		FireGameObject* fire = new FireGameObject();
		fire->pos = t->pos;
		AddGameObjectToVector(fire, gos);
		firePool.push_back(fire);
		
	}
}

void RenderFire()
{
	for (FireGameObject* fire : firePool)
	{
		if (fire->isActive)
			fire->Render();
	}
}

void FreeFire()
{
	//for (FireGameObject* fire : firePool)
	//{
	//	delete fire;
	//}
	firePool.clear();
}
