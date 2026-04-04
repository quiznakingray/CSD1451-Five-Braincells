#ifndef PAUSE_MENU
#define PAUSE_MENU

#include "SingletonTemplate.h"
#include "GameObjectManager.h"
#include "UIButton.h"

struct PauseMenu : Singleton<PauseMenu>{
	//GameObject* bg = nullptr;

	std::vector<GameObject*> pauseGameObjectVector;
	bool showConfirmation = false;
	void Load();
	void Init();
	void Update();
	void Render();
	void Free();
	void Unload();
};

struct ConfirmationMenu : Singleton<ConfirmationMenu>{
	std::vector<GameObject*> confirmationGameObjectVector;
	void Init();
	void Hide();
	void Update();
	void Render();
	void Free();
};

#endif
