#include "InputManager.h"
#include "GameStateManager.h"

void InputManager::Update()
{
	//if (AEInputCheckTriggered(interactKey))
	//{
	//	if (OnInteractionTriggered) OnInteractionTriggered();
	//}
	//if (AEInputCheckCurr(interactKey))
	//{
	//	if (OnInteractionHold) OnInteractionHold();
	//}
	//if (AEInputCheckReleased(interactKey))
	//{
	//	if (OnInteractionRelease) OnInteractionRelease();
	//}


	//pause key
	if (AEInputCheckTriggered(pauseKey))
	{
		GameStateManager::GetInstance().isGamePause = !GameStateManager::GetInstance().isGamePause;
	}
}
