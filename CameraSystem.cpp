//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "CameraSystem.h"
#include "MapManager.h"

AEVec2 CameraSystem::camPos{};
AEVec2 CameraSystem::target{};
f32 CameraSystem::camLerpSpeed = 5.f;
bool CameraSystem::lerpToTarget = false;

void CameraSystem::Init() {
	ResetCameraPosition();
	
}

void CameraSystem::Update(double dt) {

	if (lerpToTarget)
	{
		camPos.x += static_cast<f32>((target.x - camPos.x) * camLerpSpeed * dt);
		camPos.y += static_cast<f32>((target.y - camPos.y) * camLerpSpeed * dt);

		if (target.x == camPos.x && target.y == camPos.y)
		{
			lerpToTarget = false;
		}
	}

	// limit camera to map     
	float halfW = AEGfxGetWindowWidth() * 0.5f;
	float halfH = AEGfxGetWindowHeight() * 0.5f;

	size_t row = MapManager::GetInstance().GetRow();
	size_t column = MapManager::GetInstance().GetCol();

	if (row != 0 && column != 0)
	{
		Tile* topLeftTile = MapManager::GetInstance().GetTile(0, 0);
		Tile* bottomRightTile = MapManager::GetInstance().GetTile(row - 1, column - 1);

		if (topLeftTile && bottomRightTile)
		{
			// Clamp so viewport never exits the map
			float halfTile = MapManager::tileSize * 0.5f;

			// World-space edges of the map
			float mapMinX = topLeftTile->pos.x - halfTile;
			float mapMaxX = bottomRightTile->pos.x + halfTile;
			float mapMinY = bottomRightTile->pos.y - halfTile; // bottom row = lowest Y
			float mapMaxY = topLeftTile->pos.y + halfTile; // top row    = highest Y

			// Clamp camera center so viewport never exits the map
			camPos.x = AEClamp(camPos.x, mapMinX + halfW, mapMaxX - halfW);
			camPos.y = AEClamp(camPos.y, mapMinY + halfH, mapMaxY - halfH);


		}

	}
	AEGfxSetCamPosition(camPos.x, camPos.y);
}
void CameraSystem::SetCameraPos(AEVec2 pos)
{
	camPos = pos;
}

AEVec2 CameraSystem::GetCameraPos()
{
	return camPos;
}

void CameraSystem::LerpToPosition(AEVec2 pos)
{

	target = pos;
	lerpToTarget = true;

}

void CameraSystem::ResetCameraPosition()
{
	AEVec2Set(&camPos, 0.0f, 0.0f);
	lerpToTarget = false;

}
