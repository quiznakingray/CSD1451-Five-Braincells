//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef CAMERA_SYSTEM
#define CAMERA_SYSTEM
#include "AEEngine.h"
#include "SingletonTemplate.h"
struct CameraSystem
{
	static void Init();
	static void Update(double dt);

	// Set camera position directly (no lerp)
	static void SetCameraPos(AEVec2 pos);

	// Get current camera position
	static AEVec2 GetCameraPos();

	//move camera to target position over time (lerp)
	static void LerpToPosition(AEVec2 pos);

	// Instantly move camera to target position and stop lerping
	static void ResetCameraPosition();


	static AEVec2 camPos;
	static AEVec2 target; // target position for lerping

	static f32 camLerpSpeed;
	static bool lerpToTarget;
};
#endif // !CAMERA_SYSTEM
