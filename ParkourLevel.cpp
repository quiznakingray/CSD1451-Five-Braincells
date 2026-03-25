#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "EnemyManager.h"

//Player* player1 = nullptr;
PlayerManager playerManager;

std::vector<GameObject*> levelGameObjectVector{};

void ParkourLevel::Load()
{
	TextManager::Init();
}

void ParkourLevel::Init()
{
	// Clears game background
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

	switch (current) {
	case GAME_STATE_TYPE::LEVEL1:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01.csv", 0);
		break;
	case GAME_STATE_TYPE::LEVEL1BOSS:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01b.csv", 0);
		break;
	case GAME_STATE_TYPE::LEVEL2:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_02.csv", 1);
		break;
	case GAME_STATE_TYPE::LEVEL2BOSS:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_02b.csv", 0);
		break;
	default:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01.csv", 0);
		break;
	}
	
	MapManager::GetInstance().PrintMap();
	MapManager::GetInstance().AddTilesToGameObjectVector(gameObjects1);
	PlayerManager::GetInstance().Init();
	if (SaveManager::GetInstance().toContinue &&
		!SaveManager::GetInstance().playerSaveData.preserveOnLoad)
	{
		PlayerManager::GetInstance().Load();
	}

	mapManager1.AddTilesToGameObjectVector(levelGameObjectVector);

	//player->Init();
	//player1 = new Player();
	playerManager.Init();
	AddGameObjectToVector(playerManager.meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangedPlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangePlayerArrow, levelGameObjectVector);
	EnemyManager::Init(playerManager.currentPlayer);
	EnemyManager::SpawnEnemies(5, 1, levelGameObjectVector);

	InitGameObjects(levelGameObjectVector);
	
}

void ParkourLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	playerManager.Update();
	UpdateGameObjects(levelGameObjectVector);
	EnemyManager::UpdateAllEnemies(dt);
}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.6f, 0.8f, 0.85f);
	MapManager::GetInstance().DrawMapSprite();

	//player1->Render();
	playerManager.Render();
	EnemyManager::RenderEnemies();
	// enemy1->Render();
	// player1->Render();
	//enemy1->Render();

}

void ParkourLevel::Free()
{
	//playerManager.Save();

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

	MapManager::GetInstance().FreeMap();
	AEGfxSetCamPosition(0.f, 0.f);
}

void ParkourLevel::Unload()
{

}
