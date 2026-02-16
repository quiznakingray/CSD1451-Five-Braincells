#include "InputManager.h"

void InputManager::Update()
{
	if (AEInputCheckTriggered(interactKey))
	{
		if (OnInteractionTriggered) OnInteractionTriggered();
	}
	if (AEInputCheckCurr(interactKey))
	{
		if (OnInteractionHold) OnInteractionHold();
	}
	if (AEInputCheckReleased(interactKey))
	{
		if (OnInteractionRelease) OnInteractionRelease();
	}
}
