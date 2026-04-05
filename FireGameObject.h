//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef FIRE_GAME_OBJECT_H
#define FIRE_GAME_OBJECT_H

#include "GameObjectManager.h"
#include "AnimatorComponent.h"
#include "MapManager.h"

struct FireGameObject : GameObject
{
	Animation* fireAnim = nullptr;
	void Init() override;
	void Free() override;
};

void SpawnFire(std::vector<GameObject*>& gos);

void RenderFire();

void FreeFire();
#endif