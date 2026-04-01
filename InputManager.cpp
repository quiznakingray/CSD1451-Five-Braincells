#include "InputManager.h"
#include "GameStateManager.h"
#include <iostream>
#include <windows.h>
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
		GameStateManager::GetInstance().showPauseMenu = true;

	}
}

std::string InputManager::VkCodeToString(DWORD vkCode)
{
	switch (vkCode)
	{
	case VK_LBUTTON:  return "LMB";
	case VK_RBUTTON:  return "RMB";
	case VK_MBUTTON:  return "MMB";
	case VK_XBUTTON1: return "MB4";
	case VK_XBUTTON2: return "MB5";
	}

	// 1. Map Virtual Key to Scan Code
	UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);

	// 2. Adjust for extended keys
	switch (vkCode) {
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
	case VK_PRIOR: case VK_NEXT: case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE: case VK_DIVIDE:
	case VK_NUMLOCK:
		scanCode |= KF_EXTENDED;
		break;
	}

	// 3. Get key name
	char keyName[128];
	if (GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName)) > 0) {
		return std::string(keyName);
	}
	return "Unknown";
}