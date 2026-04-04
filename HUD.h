#ifndef HUD_H
#define HUD_H

#include "AEEngine.h"
#include "GameObjectManager.h"
#include "CameraSystem.h"
#include "SingletonTemplate.h"
#include "UiButton.h"

#include <vector>

struct PlayerUI {
	GameObject* border;
	GameObject* player;
	GameObject* cooldown;

	PlayerUI(GameObject* b, GameObject* p, GameObject* c) 
		: border(b), player(p), cooldown(c){

	}
	void SetCooldownText(f32 value);
	void Init(std::vector<GameObject*>& go);
};

struct HUD : Singleton<HUD>{

	void Init();
	void Update(f64 dt);
	void Render();
	void Free();

	void ShowDeathPanel();
private:
	bool showHUD = true;
	std::vector<GameObject* > HUDGameObjects;
	std::vector<GameObject* > deathHUDGameObject;

	PlayerUI* shieldPlayerUI = nullptr;

	//range player
	PlayerUI* rangePlayerUI = nullptr;


	// stamina bar
	std::vector<GameObject*> staminaBarsBG;
	std::vector<GameObject*> staminaBars;

	//health bar

	std::vector<GameObject*> healthBarsBG;
	std::vector<GameObject*> healthBars;

	bool showDeathPanel = false;
	GameObject* deathPanelBG = nullptr;
	GameObject* deathPanelTitle = nullptr;
	GameObject* deathReloadBtn = nullptr;
	GameObject* deathMainMenuBtn = nullptr;

	GameObject* sHintQ = nullptr;
	GameObject* rHintQ = nullptr;
	GameObject* rHintLMB = nullptr;

	// Top stats
	GameObject* deathCountObj = nullptr;
	GameObject* killCountObj = nullptr;
	GameObject* timerObj = nullptr;
	float       elapsedTime = 0.0f;


};

#endif
