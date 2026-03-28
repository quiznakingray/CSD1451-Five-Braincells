#include "BossLevel.h"
#include "GameObjectManager.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include "EnemyGameObject.h"
#include "EndMenu.h"

Player* player2 = nullptr;
//EnemyGameObject* enemy2 = nullptr;

std::vector<GameObject*> gameObjects2{};

MapManager mapManager2;

void BossLevel::Load()
{
	TextManager::Init();
}

void BossLevel::Init()
{
	// Clears game background
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

	mapManager2.InitMap("Assets/Maps/Map_Level_02.csv", 1);
	mapManager2.PrintMap();

	mapManager2.AddTilesToGameObjectVector(gameObjects2);

	//player->Init();
	player2 = new Player();
	//enemy2 = new EnemyGameObject();
	AddGameObjectToVector(player2, gameObjects2);
	//AddGameObjectToVector(enemy, gameObjects2);

	InitGameObjects(gameObjects2);

	EndMenu::Init();
}

void BossLevel::Update()
{

	// Check for Win Condition
	if (/* bossHP <= 0 */) { // Placeholder: Replace when you find Boss HP
		EndMenu::isWin = true;
		EndMenu::isActive = true;
	}

	// Check for Lose Condition
	if (/* playerHP <= 0 */) {
		EndMenu::isWin = false;
		EndMenu::isActive = true;
	}

	// Update Menu and intercept level logic
	if (EndMenu::isActive) {
		EndMenu::Update();
		return;
	}

	double dt = AEFrameRateControllerGetFrameTime();
	UpdateGameObjects(gameObjects2);

	// Check for Death (Calls the shared logic)
	CheckPlayerDeath();

	// 4. Check for Boss Win (Level specific)
	if (bossHP <= 0) {
		EndMenu::isWin = true;
		EndMenu::isActive = true;
	}
}

void BossLevel::Render()
{

	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager2.DrawMapSprite();

	player2->Render();
	//enemy->Render();

}

void BossLevel::Free()
{
	// Clean up game objects
	for (auto* obj : gameObjects2)
	{
		if (obj && obj != player2)
			delete obj;
	}
	gameObjects2.clear();

	// Clean up player2
	if (player2)
	{
		delete player2;
		player2 = nullptr;
	}

	mapManager2.FreeMap();
	AEGfxSetCamPosition(0.f, 0.f);

	EndMenu::Free();
	EndMenu::isActive = false; // Reset for the next time the level loads
}

void BossLevel::Unload()
{

}
