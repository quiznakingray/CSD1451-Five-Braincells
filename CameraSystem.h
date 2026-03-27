#ifndef CAMERA_SYSTEM
#define CAMERA_SYSTEM
#include "AEEngine.h"
#include "SingletonTemplate.h"
struct CameraSystem
{
	static void Init();
	static void Update(float dt);

	static void SetCameraPos(AEVec2 pos);
	static AEVec2 GetCameraPos();
	static void LerpToPosition(AEVec2 pos);

	static void ResetCameraPosition();


	static AEVec2 camPos;
	static AEVec2 target;

	static f32 camLerpSpeed;
	static bool lerpToTarget;
};
#endif // !CAMERA_SYSTEM
