#include "HUD.h"
#include "SpriteManager.h"
#include "GameObjectManager.h"  
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "PlayerStats.h"
#include "TextComponent.h"
#include "InputManager.h"
#include "TextManager.h"
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
	if (!HUDGameObjects.empty())
		Free();

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
	sSprite->textureFileName = "Assets/SpriteSheets/Player_Melee_Mugshot.png";
	Collider* sCollider = shieldPlayer->AddComponent(new Collider());
	sCollider->canInteract = true;
	sCollider->OnClick = [] {
		PlayerManager::GetInstance().ChangePlayer(PLAYER_TYPE::MELEE);
		};	

	Sprite* sButtonSprite = shieldPlayer->AddComponent(new Sprite);
	sButtonSprite->meshColor = 0xFF000000;
	sButtonSprite->offset = { 0, playerUIScale.y / 2.f };
	sButtonSprite->size = { 0.25f, 0.25f};

	Text* sText = shieldPlayer->AddComponent(new Text());
	sText->inWorldSpace = false;
	sText->center.y = playerUIScale.y / 2.f;
	sText->SetText(InputManager::VkCodeToString(AEVK_E));

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
	s->textureFileName = "Assets/SpriteSheets/Player_Range_Mugshot.png";
	Collider* c = rangePlayer->AddComponent(new Collider());
	c->canInteract = true;
	c->OnClick = [] {
		PlayerManager::GetInstance().ChangePlayer(PLAYER_TYPE::RANGE);
		};	
	Sprite* rButtonSprite = rangePlayer->AddComponent(new Sprite);
	rButtonSprite->meshColor = 0xFF000000;
	rButtonSprite->offset = { 0, playerUIScale.y / 2.f };
	rButtonSprite->size = { 0.25f, 0.25f };

	Text* rText = rangePlayer->AddComponent(new Text());
	rText->inWorldSpace = false;
	rText->center.y = playerUIScale.y / 2.f;
	rText->SetText(InputManager::VkCodeToString(AEVK_R));

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

	// --- SHIELD PLAYER ACTION HINTS ---
	AEVec2 sHintPos = {AEGfxGetWindowWidth() / 2.f - 200, playerUIStartPos.y };

	// Q - Hold hint
	sHintQ = new GameObject(400, 50, sHintPos.x, sHintPos.y, playerUIStartPos.z, 0, true);
	sHintQ->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* sHintQText = sHintQ->AddComponent(new Text());
	sHintQText->inWorldSpace = false;
	sHintQText->SetText("[" + InputManager::VkCodeToString(AEVK_Q) + "] Hold - Shield");
	AddGameObjectToVector(sHintQ, HUDGameObjects);

	// --- RANGE PLAYER ACTION HINTS ---
	AEVec2 rHintPos = { sHintPos.x, playerUIStartPos.y};

	// Q - Hold hint
	rHintQ = new GameObject(400, 50, rHintPos.x, rHintPos.y, playerUIStartPos.z, 0, true);
	rHintQ->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* rHintQText = rHintQ->AddComponent(new Text());
	rHintQText->inWorldSpace = false;
	rHintQText->SetText("[" + InputManager::VkCodeToString(AEVK_Q) + "] Hold - Aim");
	AddGameObjectToVector(rHintQ, HUDGameObjects);

	// LMB - Click hint
	rHintLMB = new GameObject(400, 50, rHintPos.x, rHintPos.y - 45.f, playerUIStartPos.z, 0, true);
	rHintLMB->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* rHintLMBText = rHintLMB->AddComponent(new Text());
	rHintLMBText->inWorldSpace = false;
	rHintLMBText->SetText("[" + InputManager::VkCodeToString(AEVK_LBUTTON) + "] - Shoot");
	AddGameObjectToVector(rHintLMB, HUDGameObjects);

	// -------------------------------------------------------
	// TOP STATS (deaths, kills, timer)
	// -------------------------------------------------------
	float topY = AEGfxGetWindowHeight() / 2.0f - 40.f;
	float leftX = -AEGfxGetWindowWidth() / 2.0f + 120.f;
	float rightX = AEGfxGetWindowWidth() / 2.0f - 120.f;

	deathCountObj = new GameObject(200, 40, leftX, topY, 1, 0, true);
	deathCountObj->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* deathText = deathCountObj->AddComponent(new Text());
	deathText->inWorldSpace = false;
	deathText->SetText("Deaths: 0");
	AddGameObjectToVector(deathCountObj, HUDGameObjects);

	killCountObj = new GameObject(200, 40, leftX, topY - 50.f, 1, 0, true);
	killCountObj->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* killText = killCountObj->AddComponent(new Text());
	killText->inWorldSpace = false;
	killText->SetText("Kills: 0");
	AddGameObjectToVector(killCountObj, HUDGameObjects);

	timerObj = new GameObject(200, 40, rightX, topY, 1, 0, true);
	timerObj->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* timerText = timerObj->AddComponent(new Text());
	timerText->inWorldSpace = false;
	timerText->SetText("0:00");
	AddGameObjectToVector(timerObj, HUDGameObjects);
	InitGameObjects(HUDGameObjects);
}
void HUD::Update(f64 dt)
{
	// update players
	bool isMelee = PlayerManager::GetInstance().currentPlayerType == PLAYER_TYPE::MELEE;
	bool isRange = PlayerManager::GetInstance().currentPlayerType == PLAYER_TYPE::RANGE;

	shieldPlayerUI->border->isActive = isMelee;
	rangePlayerUI->border->isActive = isRange;

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

	sHintQ->isActive = isMelee;
	rHintQ->isActive = isRange;
	rHintLMB->isActive = isRange;
	UpdateGameObjects(HUDGameObjects);

	// --- TOP STATS ---
	PlayerStats::Get().SetTotalSeconds(PlayerStats::Get().GetTotalSeconds() + dt);
	int totalSeconds = static_cast<int>(PlayerStats::Get().GetTotalSeconds());
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	char timerBuf[16];
	sprintf_s(timerBuf, "%d:%02d", minutes, seconds);

	auto setText = [](GameObject* obj, const std::string& str) {
		std::vector<Text*> texts = obj->GetComponents<Text>();
		if (!texts.empty()) texts[0]->SetText(str);
		};

	setText(timerObj, timerBuf);
	setText(deathCountObj, "Deaths: " + std::to_string(PlayerStats::Get().deathCount));
	setText(killCountObj, "Kills: " + std::to_string(PlayerStats::Get().killCount));

}

void HUD::Render() {
	if (!showHUD) return;
	RenderGameObjects(HUDGameObjects);

	//AEGfxPrint(TextManager::pFont, "help", -1, 0, 1, 1, 1, 1, 1);
}

void HUD::Free() {
	elapsedTime = 0.0f;
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

	deathCountObj = nullptr;
	killCountObj = nullptr;
	timerObj = nullptr;
	sHintQ = nullptr;
	rHintQ = nullptr;
	rHintLMB = nullptr;
}