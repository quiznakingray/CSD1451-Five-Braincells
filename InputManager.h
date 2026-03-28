#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "AEEngine.h"
#include "SingletonTemplate.h"
#include "GameObjectManager.h"

#include <functional>

struct InputManager : public Singleton <InputManager> {

	u8 interactKey = AEVK_F;

	std::function<void()> OnInteractionTriggered = nullptr;
	std::function<void()> OnInteractionHold;
	std::function<void()> OnInteractionRelease;

	void Update();

};
#endif // INPUT_MANAGER_H
