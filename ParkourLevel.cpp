#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "EnemyManager.h"

//Player* player1 = nullptr;
PlayerManager playerManager;

std::vector<GameObject*> levelGameObjectVector{};

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
	

	mapManager1.AddTilesToGameObjectVector(levelGameObjectVector);

	//player->Init();
	//player1 = new Player();
	playerManager.Init();
	AddGameObjectToVector(playerManager.meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangedPlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangePlayerArrow, levelGameObjectVector);
	EnemyManager::get_instance().Init(playerManager.currentPlayer);
	EnemyManager::get_instance().SpawnEnemies(5, 1, levelGameObjectVector);

	InitGameObjects(levelGameObjectVector);
	
}

void ParkourLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	playerManager.Update();
	UpdateGameObjects(levelGameObjectVector);
	EnemyManager::get_instance().UpdateAllEnemies(dt);
}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager1.DrawMapSprite();

	//player1->Render();
	playerManager.Render();
	EnemyManager::get_instance().RenderEnemies();
	// enemy1->Render();
	// player1->Render();
	//enemy1->Render();

}

void ParkourLevel::Free()
{
	// Clean up game objects
	for (auto* obj : levelGameObjectVector)
	{
		if (obj )
			delete obj;
	}
	levelGameObjectVector.clear();

	//// Clean up player1
	//if (player1)
	//{
	//	delete player1;
	//	player1 = nullptr;
	//}

	mapManager1.FreeMap();
	AEGfxSetCamPosition(0.f, 0.f);
}

void ParkourLevel::Unload()
{

}
