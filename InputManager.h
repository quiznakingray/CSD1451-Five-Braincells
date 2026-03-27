#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "AEEngine.h"
#include "SingletonTemplate.h"
#include "GameObjectManager.h"

#include <functional>

struct InputManager : Singleton <InputManager> {

	u8 interactKey = AEVK_F;
	u8 pauseKey = AEVK_ESCAPE;

	void Update();

};
#endif // INPUT_MANAGER_H
