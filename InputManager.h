//---------------------------------------------------------
// author:    Choy Phui Mun 
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "AEEngine.h"
#include "SingletonTemplate.h"
#include "GameObjectManager.h"

#include <functional>
#include <string>

struct InputManager : Singleton <InputManager> {

	u8 interactKey = AEVK_F;
	u8 pauseKey = AEVK_ESCAPE;

	void Update();
	static std::string VkCodeToString(DWORD vkCode);
};
#endif // INPUT_MANAGER_H
