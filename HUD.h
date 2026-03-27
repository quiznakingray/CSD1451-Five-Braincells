#ifndef HUD_H
#define HUD_H

#include "AEEngine.h"
#include "GameObjectManager.h"
#include "CameraSystem.h"
#include "SingletonTemplate.h"

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
	void Update(f32 dt);
	void Render();
	void Free();
private:
	bool showHUD = true;
	std::vector<GameObject* > HUDGameObjects;

	PlayerUI* shieldPlayerUI;

	//range player
	PlayerUI* rangePlayerUI;


	// stamina bar
	std::vector<GameObject*> staminaBarsBG;
	std::vector<GameObject*> staminaBars;

	//health bar

	std::vector<GameObject*> healthBarsBG;
	std::vector<GameObject*> healthBars;

};

#endif
