#include "HUD.h"
#include "SpriteManager.h"
#include "GameObjectManager.h"  
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "PlayerStats.h"
#include "TextComponent.h"
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

void PlayerUI::SetCooldownText(f32 value)
{
	if (!cooldown) return;
	std::vector<Text*> textComponents = cooldown->GetComponents<Text>();
	if (textComponents.size() > 0)
	{
		// Create a stringstream object
		std::stringstream ss;

		// Use std::fixed to format with a fixed number of decimal places
		// Use std::setprecision(2) to specify exactly 2 digits after the decimal point
		ss << std::fixed << std::setprecision(1) << value;

		// Get the string object from the stringstream
		std::string result = ss.str();
		textComponents[0]->SetText(result);
	}
}

void PlayerUI::Init(std::vector<GameObject*>& go)
{
	AddGameObjectToVector(border, go);
	AddGameObjectToVector(player, go);
	AddGameObjectToVector(cooldown, go);
}

void HUD::Init()
{
	// shieldPlayerIO
	AEVec3 playerUIStartPos;
	AEVec2 playerUIScale;
	AEVec2Set(&playerUIScale, 200, 200);
	AEVec2Set(&playerUIStartPos,
		-AEGfxGetWindowWidth() / 2.0f + playerUIScale.x/ 2.0f + 30,
		-AEGfxGetWindowHeight() / 2.0f + playerUIScale.y / 2.0f + 30);
	playerUIStartPos.z = 0;
	// shield player UI Border
	GameObject* shieldPlayerUIBorder = new GameObject(playerUIScale.x + 20, playerUIScale.y + 20, playerUIStartPos.x, playerUIStartPos.y, playerUIStartPos.z, 0, true);
	Sprite* sBorderSprite = shieldPlayerUIBorder->AddComponent(new Sprite());
	sBorderSprite->meshColor = 0xFFFF0000;
	//AddGameObjectToVector(shieldPlayerUIBorder, HUDGameObjects);

	// shield player UI
	GameObject* shieldPlayer = new GameObject(playerUIScale.x, playerUIScale.y, playerUIStartPos.x, playerUIStartPos.y, playerUIStartPos.z, 0, true);
	Sprite* sSprite = shieldPlayer->AddComponent(new Sprite());
	sSprite->meshColor = 0xFF00FF00;
	Collider* sCollider = shieldPlayer->AddComponent(new Collider());
	sCollider->canInteract = true;
	sCollider->OnClick = [] {
		PlayerManager::GetInstance().ChangePlayer(PLAYER_TYPE::MELEE);
		};	


	//cooldown overlay (shield)
	GameObject* shieldPlayerCooldown = new GameObject(playerUIScale.x, playerUIScale.y, playerUIStartPos.x, playerUIStartPos.y, playerUIStartPos.z, 0, true);
	Sprite* sSpriteCooldown = shieldPlayerCooldown->AddComponent(new Sprite());
	sSpriteCooldown->meshColor = 0xAA000000;
	
	shieldPlayerUI = new PlayerUI(shieldPlayerUIBorder, shieldPlayer, shieldPlayerCooldown);
	Text* shieldCooldown = shieldPlayerCooldown->AddComponent(new Text());
	shieldCooldown->inWorldSpace = false;
	//shieldCooldown->center.y = 10.f;
	shieldPlayerUI->Init(HUDGameObjects);


	// range player UI Border
	GameObject* rangePlayerUIBorder = new GameObject(playerUIScale.x + 20, playerUIScale.y + 20, 
		playerUIStartPos.x + playerUIScale.x + 70, playerUIStartPos.y, playerUIStartPos.z, 0, true);
	Sprite* rBorderSprite = rangePlayerUIBorder->AddComponent(new Sprite());
	rBorderSprite->meshColor = 0xFFFF0000;
	

	// range player UI
	GameObject* rangePlayer = new GameObject(playerUIScale.x, playerUIScale.y,
		playerUIStartPos.x + playerUIScale.x + 70, playerUIStartPos.y, playerUIStartPos.z, 0, true);
	Sprite* s = rangePlayer->AddComponent(new Sprite());
	s->meshColor = 0xFF00FF00;
	Collider* c = rangePlayer->AddComponent(new Collider());
	c->canInteract = true;
	c->OnClick = [] {
		PlayerManager::GetInstance().ChangePlayer(PLAYER_TYPE::RANGE);
		};	

	GameObject* rangePlayerCooldown = new GameObject(playerUIScale.x, playerUIScale.y, playerUIStartPos.x + playerUIScale.x + 70, playerUIStartPos.y, playerUIStartPos.z, 0, true);
	Sprite* rSpriteCooldown = rangePlayerCooldown->AddComponent(new Sprite());
	rSpriteCooldown->meshColor = 0xAA000000;
	Text* rangeCooldown = rangePlayerCooldown->AddComponent(new Text());
	rangeCooldown->inWorldSpace = false;
	
	rangePlayerUI = new PlayerUI(rangePlayerUIBorder, rangePlayer, rangePlayerCooldown);

	rangePlayerUI->Init(HUDGameObjects);




	// stamina bar
	AEVec2 staminaBarStartPos, staminaBarScale;
	AEVec2Set(&staminaBarStartPos, -AEGfxGetWindowWidth() / 2.0f + 600, -AEGfxGetWindowHeight() / 2.0f + 80);
	AEVec2Set(&staminaBarScale, 100, 50);
	for (int i = 0; i < PlayerStats::Get().maxJumpStamina; i++)
	{
		f32 posX = staminaBarStartPos.x + i * (staminaBarScale.x + 30);
		//bar bg
		GameObject* barBG = new GameObject(staminaBarScale.x, staminaBarScale.y, posX, staminaBarStartPos.y, 1, 0, true);
		Sprite* barBGSprite = barBG->AddComponent(new Sprite());
		barBGSprite->meshColor = 0xFFb39305;
		staminaBarsBG.push_back(barBG);
		AddGameObjectToVector(barBG, HUDGameObjects);

		// bar
		GameObject* bar = new GameObject(staminaBarScale.x, staminaBarScale.y, posX,staminaBarStartPos.y, 1, 0, true);
		Sprite* s = bar->AddComponent(new Sprite());
		s->meshColor = 0xFFFFDD00;
		staminaBars.push_back(bar);

		AddGameObjectToVector(bar, HUDGameObjects);
	}

	AEVec2 healthBarStartPos, healthBarScale;
	AEVec2Set(&healthBarStartPos, -AEGfxGetWindowWidth() / 2.0f + 588, -AEGfxGetWindowHeight() / 2.0f + 160);
	AEVec2Set(&healthBarScale, 75, 75);
	for (int i = 0; i < PlayerStats::Get().maxHealth; i++)
	{
		f32 posX = healthBarStartPos.x + i * 100;
		//bar bg
		GameObject* barBG = new GameObject(healthBarScale.x, healthBarScale.y, posX, healthBarStartPos.y, 1, 0, true);
		Sprite* barBGSprite = barBG->AddComponent(new Sprite());
		barBGSprite->meshColor = 0xFF640707;
		healthBarsBG.push_back(barBG);
		AddGameObjectToVector(barBG, HUDGameObjects);

		// bar
		GameObject* bar = new GameObject(healthBarScale.x, healthBarScale.y, posX,healthBarStartPos.y, 1, 0, true);
		Sprite* s = bar->AddComponent(new Sprite());
		s->meshColor = 0xFFdb0b0b;
		healthBars.push_back(bar);

		AddGameObjectToVector(bar, HUDGameObjects);
	}
	InitGameObjects(HUDGameObjects);
}
void HUD::Update(f64 dt)
{
	// update players
	shieldPlayerUI->border->isActive = (PlayerManager::GetInstance().currentPlayerType == PLAYER_TYPE::MELEE);
	rangePlayerUI->border->isActive = (PlayerManager::GetInstance().currentPlayerType == PLAYER_TYPE::RANGE);

	shieldPlayerUI->cooldown->isActive = rangePlayerUI->cooldown->isActive = !PlayerManager::GetInstance().canChangePlayer;
	
	shieldPlayerUI->SetCooldownText(PlayerManager::GetInstance().playerSwitchingCooldown);
	rangePlayerUI->SetCooldownText(PlayerManager::GetInstance().playerSwitchingCooldown);
	//update stamina
	for (int i = 0; i < staminaBars.size(); i++)
	{
		staminaBars[i]->isActive = (PlayerStats::Get().jumpStamina >= i + 1);
		staminaBarsBG[i]->isActive = (PlayerStats::Get().maxJumpStamina >= i + 1);
	}

	//update health
	for (int i = 0; i < healthBars.size(); i++)
	{
		healthBars[i]->isActive = (PlayerStats::Get().health >= i + 1);
	}
	UpdateGameObjects(HUDGameObjects);

}

void HUD::Render() {
	if (!showHUD) return;
	RenderGameObjects(HUDGameObjects);

	//AEGfxPrint(TextManager::pFont, "help", -1, 0, 1, 1, 1, 1, 1);
}

void HUD::Free() {
	FreeGameObjects(HUDGameObjects);
	for (GameObject* go : HUDGameObjects) {
		delete go;
	}
	HUDGameObjects.clear();
	staminaBarsBG.clear();   // non-owning, just clear
	staminaBars.clear();     // non-owning, just clear
	healthBarsBG.clear();    // non-owning, just clear
	healthBars.clear();      // non-owning, just clear

	// delete PlayerUI structs themselves
	delete shieldPlayerUI;
	shieldPlayerUI = nullptr;
	delete rangePlayerUI;
	rangePlayerUI = nullptr;
}