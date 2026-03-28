#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "EndMenu.h"

//Player* player1 = nullptr;
PlayerManager playerManager;
EnemyGameObject* enemy1 = nullptr;

std::vector<GameObject*> gameObjects1{};

MapManager mapManager1;

void ParkourLevel::Load()
{
	TextManager::Init();
}

void ParkourLevel::Init()
{
	// Clears game background
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

	mapManager1.InitMap("Assets/Maps/Map_Level_01.csv", 0);
	mapManager1.PrintMap();
	

	mapManager1.AddTilesToGameObjectVector(gameObjects1);

	//player->Init();
	//player1 = new Player();
	playerManager.Init();
	enemy1 = new EnemyGameObject();
	AddGameObjectToVector(playerManager.meleePlayer, gameObjects1);
	AddGameObjectToVector(playerManager.rangedPlayer, gameObjects1);
	AddGameObjectToVector(playerManager.rangePlayerArrow, gameObjects1);
	AddGameObjectToVector(enemy1, gameObjects1);

	InitGameObjects(gameObjects1);

	EndMenu::Init();
	
}

void ParkourLevel::Update()
{
	// 1. Check if the player is dead (Example using PlayerStats if available)
	if (/* playerHP <= 0 */) { // Placeholder: Replace with your actual HP variable
		EndMenu::isWin = false;
		EndMenu::isActive = true;
	}

	// 2. Run Menu Update and skip level logic if active
	if (EndMenu::isActive) {
		EndMenu::Update();
		return;
	}
	double dt = AEFrameRateControllerGetFrameTime();
	playerManager.Update();
	UpdateGameObjects(gameObjects1);

}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager1.DrawMapSprite();

	//player1->Render();
	playerManager.Render();
	enemy1->Render();

	EndMenu::Render();

}

void ParkourLevel::Free()
{
	// Clean up game objects
	for (auto* obj : gameObjects1)
	{
		if (obj )
			delete obj;
	}
	gameObjects1.clear();

	//// Clean up player1
	//if (player1)
	//{
	//	delete player1;
	//	player1 = nullptr;
	//}

	mapManager1.FreeMap();
	AEGfxSetCamPosition(0.f, 0.f);

	EndMenu::Free();
	EndMenu::isActive = false; // Reset for the next time the level loads
}

void ParkourLevel::Unload()
{

}
