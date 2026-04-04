#include "HUD.h"
#include "SpriteManager.h"
#include "GameObjectManager.h"  
#include "GameStateManager.h"  
#include "CollisionManager.h"
#include "PlayerManager.h"
#include "PlayerStats.h"
#include "TextComponent.h"
#include "InputManager.h"
#include "TextManager.h"
#include "SaveManager.h"
#include "LoadingScreen.h"
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

void HUD::ShowDeathPanel()
{

	showDeathPanel = true;
	GameStateManager::GetInstance().gamePaused = true;

	//deathPanelBG->isActive = true;
	//deathPanelTitle->isActive = true;

	//deathReloadBtn->isActive = true;
	//deathMainMenuBtn->isActive = true;
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
		-AEGfxGetWindowWidth() / 2.0f + playerUIScale.x / 2.0f + 30,
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
	sButtonSprite->size = { 0.25f, 0.25f };

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
	for (int i = 0; i < PlayerStats::GetInstance().maxJumpStamina; i++)
	{
		f32 posX = staminaBarStartPos.x + i * (staminaBarScale.x + 30);
		//bar bg
		GameObject* barBG = new GameObject(staminaBarScale.x, staminaBarScale.y, posX, staminaBarStartPos.y, 1, 0, true);
		Sprite* barBGSprite = barBG->AddComponent(new Sprite());
		barBGSprite->meshColor = 0xFFb39305;
		staminaBarsBG.push_back(barBG);
		AddGameObjectToVector(barBG, HUDGameObjects);

		// bar
		GameObject* bar = new GameObject(staminaBarScale.x, staminaBarScale.y, posX, staminaBarStartPos.y, 1, 0, true);
		Sprite* sBar = bar->AddComponent(new Sprite());
		sBar->meshColor = 0xFFFFDD00;
		staminaBars.push_back(bar);

		AddGameObjectToVector(bar, HUDGameObjects);
	}

	AEVec2 healthBarStartPos, healthBarScale;
	AEVec2Set(&healthBarStartPos, -AEGfxGetWindowWidth() / 2.0f + 588, -AEGfxGetWindowHeight() / 2.0f + 160);
	AEVec2Set(&healthBarScale, 75, 75);
	for (int i = 0; i < PlayerStats::GetInstance().maxHealth; i++)
	{
		f32 posX = healthBarStartPos.x + i * 100;
		//bar bg
		GameObject* barBG = new GameObject(healthBarScale.x, healthBarScale.y, posX, healthBarStartPos.y, 1, 0, true);
		Sprite* barBGSprite = barBG->AddComponent(new Sprite());
		barBGSprite->meshColor = 0xFF640707;
		healthBarsBG.push_back(barBG);
		AddGameObjectToVector(barBG, HUDGameObjects);

		// bar
		GameObject* bar = new GameObject(healthBarScale.x, healthBarScale.y, posX, healthBarStartPos.y, 1, 0, true);
		Sprite* sBar = bar->AddComponent(new Sprite());
		sBar->meshColor = 0xFFdb0b0b;
		healthBars.push_back(bar);

		AddGameObjectToVector(bar, HUDGameObjects);
	}

	// --- SHIELD PLAYER ACTION HINTS ---
	AEVec2 sHintPos = { AEGfxGetWindowWidth() / 2.f - 200, playerUIStartPos.y };

	// Q - Hold hint
	sHintQ = new GameObject(400, 50, sHintPos.x, sHintPos.y, playerUIStartPos.z, 0, true);
	sHintQ->AddComponent(new Sprite())->meshColor = 0xFF333333;
	Text* sHintQText = sHintQ->AddComponent(new Text());
	sHintQText->inWorldSpace = false;
	sHintQText->SetText("[" + InputManager::VkCodeToString(AEVK_Q) + "] Hold - Shield");
	AddGameObjectToVector(sHintQ, HUDGameObjects);

	// --- RANGE PLAYER ACTION HINTS ---
	AEVec2 rHintPos = { sHintPos.x, playerUIStartPos.y };

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

	// -------------------------------------------------------
	// DEATH PANEL (hidden until ShowDeathPanel() is called)
	// -------------------------------------------------------
	float panelW = 500.f, panelH = 300.f;
	float panelX = 0.f, panelY = 0.f; // screen centre

	// Dark backdrop
	GameObject* deathBG = new GameObject(AEGfxGetWindowWidth(), AEGfxGetWindowHeight(), panelX, panelY, 2, 0, true);
	deathBG->AddComponent(new Sprite())->meshColor = 0x77000000;
	AddGameObjectToVector(deathBG, deathHUDGameObject);

	// Dark panel
	deathPanelBG = new GameObject(panelW, panelH, panelX, panelY, 2, 0, true);
	deathPanelBG->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/audio_panel.png";
	AddGameObjectToVector(deathPanelBG, deathHUDGameObject);

	// "YOU DIED" title
	deathPanelTitle = new GameObject(panelW - 20.f, 60.f, panelX, panelY + 90.f, 2, 0, true);
	deathPanelTitle->AddComponent(new Sprite())->meshColor = 0x00000000; // transparent bg
	Text* titleText = deathPanelTitle->AddComponent(new Text());
	titleText->inWorldSpace = false;
	titleText->SetText("YOU DIED");
	AddGameObjectToVector(deathPanelTitle, deathHUDGameObject);

	// RELOAD SAVE button
	deathReloadBtn = new GameObject(200.f, 55.f, panelX - 120.f, panelY - 60.f, 2, 0, true);
	deathReloadBtn->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/button_idle.png";

	Collider* reloadCol = deathReloadBtn->AddComponent(new Collider());
	reloadCol->canInteract = true;
	reloadCol->OnClick = [] {
		if (SaveManager::GetInstance().HasSaveData()) {
			SaveManager::GetInstance().LoadPlayerData();
			SaveManager::GetInstance().LoadMapData();
			SaveManager::GetInstance().LoadEnemyData();
			SaveManager::GetInstance().toContinue = true;

			GAME_STATE_TYPE respawnLevel = SaveManager::GetInstance().mapSaveData.savedLevel;

			current = GAME_STATE_TYPE::MENU;
			LoadingScreen::targetState = respawnLevel;
			GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
			GameStateManager::GetInstance().gamePaused = false;
			HUD::GetInstance().showDeathPanel = false;
		}
		};
	reloadCol->OnMouseEnter = [this]() {
		deathReloadBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_hover.png";
		};
	reloadCol->OnMouseExit = [this]() {
		deathReloadBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
		};

	Text* reloadText = deathReloadBtn->AddComponent(new Text());
	reloadText->inWorldSpace = false;
	reloadText->SetText("RELOAD SAVE");
	AddGameObjectToVector(deathReloadBtn, deathHUDGameObject);

	// MAIN MENU button
	deathMainMenuBtn = new GameObject(200.f, 55.f, panelX + 120.f, panelY - 60.f, 2, 0, true);
	deathMainMenuBtn->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/button_idle.png";

	Collider* menuCol = deathMainMenuBtn->AddComponent(new Collider());
	menuCol->canInteract = true;
	menuCol->OnMouseUp = [] {
		HUD::GetInstance().showDeathConfirm = true;
		HUD::GetInstance().deathConfirmBG->isActive = true;
		HUD::GetInstance().deathConfirmLabel->isActive = true;
		HUD::GetInstance().deathConfirmYesBtn->isActive = true;
		HUD::GetInstance().deathConfirmNoBtn->isActive = true;
		};
	menuCol->OnMouseEnter = [this]() {
		deathMainMenuBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_hover.png";
		};
	menuCol->OnMouseExit = [this]() {
		deathMainMenuBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
		};

	Text* menuText = deathMainMenuBtn->AddComponent(new Text());
	menuText->inWorldSpace = false;
	menuText->SetText("MAIN MENU");

	AddGameObjectToVector(deathMainMenuBtn, deathHUDGameObject);

	// confirmation to go to main menu after clicking on main menu option
	deathConfirmBG = new GameObject(panelW, panelH, panelX, panelY, 3, 0, true);
	deathConfirmBG->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/audio_panel.png";
	deathConfirmBG->isActive = false;
	AddGameObjectToVector(deathConfirmBG, deathHUDGameObject);

	deathConfirmLabel = new GameObject(panelW - 20.f, 60.f, panelX, panelY + 90.f, 3, 0, true);
	//deathConfirmLabel->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
	Text* confirmLabelText = deathConfirmLabel->AddComponent(new Text());
	confirmLabelText->inWorldSpace = false;
	confirmLabelText->SetText("GO TO MAIN MENU?");
	deathConfirmLabel->isActive = false;
	AddGameObjectToVector(deathConfirmLabel, deathHUDGameObject);

	// NO - go back to the death panel
	deathConfirmNoBtn = new GameObject(200.f, 55.f, panelX - 120.f, panelY - 60.f, 3, 0, true);
	deathConfirmNoBtn->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
	Collider* confirmNoCol = deathConfirmNoBtn->AddComponent(new Collider());
	confirmNoCol->canInteract = true;
	confirmNoCol->OnMouseUp = [] {
		HUD::GetInstance().showDeathConfirm = false;
		HUD::GetInstance().deathConfirmBG->isActive = false;
		HUD::GetInstance().deathConfirmLabel->isActive = false;
		HUD::GetInstance().deathConfirmYesBtn->isActive = false;
		HUD::GetInstance().deathConfirmNoBtn->isActive = false;
		};
	confirmNoCol->OnMouseEnter = [this]() {
		deathConfirmNoBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_hover.png";
		};
	confirmNoCol->OnMouseExit = [this]() {
		deathConfirmNoBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
		};

	Text* confirmNoText = deathConfirmNoBtn->AddComponent(new Text());
	confirmNoText->inWorldSpace = false;
	confirmNoText->SetText("NO");
	deathConfirmNoBtn->isActive = false;
	AddGameObjectToVector(deathConfirmNoBtn, deathHUDGameObject);

	// YES - confirmed, go to main menu
	deathConfirmYesBtn = new GameObject(200.f, 55.f, panelX + 120.f, panelY - 60.f, 3, 0, true);
	deathConfirmYesBtn->AddComponent(new Sprite())->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
	Collider* confirmYesCol = deathConfirmYesBtn->AddComponent(new Collider());
	confirmYesCol->canInteract = true;
	confirmYesCol->OnMouseUp = [] {
		LoadingScreen::targetState = GAME_STATE_TYPE::MENU;
		GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
		GameStateManager::GetInstance().gamePaused = false;
		HUD::GetInstance().showDeathPanel = false;
		};
	confirmYesCol->OnMouseEnter = [this]() {
		deathConfirmYesBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_hover.png";
		};
	confirmYesCol->OnMouseExit = [this]() {
		deathConfirmYesBtn->GetComponent<Sprite>()->textureFileName = "Assets/TEMP_Sprites/button_idle.png";
		};
	Text* confirmYesText = deathConfirmYesBtn->AddComponent(new Text());
	confirmYesText->inWorldSpace = false;
	confirmYesText->SetText("YES");
	deathConfirmYesBtn->isActive = false;
	AddGameObjectToVector(deathConfirmYesBtn, deathHUDGameObject);

	InitGameObjects(HUDGameObjects);
	InitGameObjects(deathHUDGameObject);
}
void HUD::Update(f64 dt)
{

	if (showDeathPanel) {
		for (int i = 0; i < healthBars.size(); i++)
		{
			healthBars[i]->isActive = (PlayerStats::GetInstance().health >= i + 1);
		}

		// hide the death panel buttons so they can't be clicked through the overlay
		deathReloadBtn->isActive = !showDeathConfirm;
		deathMainMenuBtn->isActive = !showDeathConfirm;

		UpdateGameObjects(deathHUDGameObject);
		return;
	}
	if (GameStateManager::GetInstance().gamePaused) return;
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
		staminaBars[i]->isActive = (PlayerStats::GetInstance().jumpStamina >= i + 1);
		staminaBarsBG[i]->isActive = (PlayerStats::GetInstance().maxJumpStamina >= i + 1);
	}

	//update health
	for (int i = 0; i < healthBars.size(); i++)
	{
		healthBars[i]->isActive = (PlayerStats::GetInstance().health >= i + 1);
	}

	sHintQ->isActive = isMelee;
	rHintQ->isActive = isRange;
	rHintLMB->isActive = isRange;
	UpdateGameObjects(HUDGameObjects);

	// --- TOP STATS ---
	PlayerStats::GetInstance().SetTotalSeconds(PlayerStats::GetInstance().GetTotalSeconds() + static_cast<float>(dt));
	int totalSeconds = static_cast<int>(PlayerStats::GetInstance().GetTotalSeconds());
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;
	char timerBuf[16];
	sprintf_s(timerBuf, "%d:%02d", minutes, seconds);

	auto setText = [](GameObject* obj, const std::string& str) {
		std::vector<Text*> texts = obj->GetComponents<Text>();
		if (!texts.empty()) texts[0]->SetText(str);
		};

	setText(timerObj, timerBuf);
	setText(deathCountObj, "Deaths: " + std::to_string(PlayerStats::GetInstance().deathCount));
	setText(killCountObj, "Kills: " + std::to_string(PlayerStats::GetInstance().killCount));

}

void HUD::Render() {
	if (!showHUD) return;
	RenderGameObjects(HUDGameObjects);
	if(showDeathPanel) RenderGameObjects(deathHUDGameObject);
	//AEGfxPrint(TextManager::pFont, "help", -1, 0, 1, 1, 1, 1, 1);
}

void HUD::Free() {
	elapsedTime = 0.0f;
	FreeGameObjects(HUDGameObjects);
	for (GameObject* go : HUDGameObjects) {
		delete go;
	}
	FreeGameObjects(deathHUDGameObject);
	for (GameObject* go : deathHUDGameObject) {
		delete go;
	}
	HUDGameObjects.clear();
	deathHUDGameObject.clear();
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
	deathPanelBG = nullptr;
	deathPanelTitle = nullptr;
	deathReloadBtn = nullptr;
	deathMainMenuBtn = nullptr;
	deathConfirmBG = nullptr;
	deathConfirmLabel = nullptr;
	deathConfirmYesBtn = nullptr;
	deathConfirmNoBtn = nullptr;
	showDeathPanel = false;
	showDeathConfirm = false;
}