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
	
	//MapManager::GetInstance().PrintMap();
	PlayerManager::GetInstance().Init();
	MapManager::GetInstance().AddTilesToGameObjectVector(levelGameObjectVector);
	//player->Init();
	//player1 = new Player();
	playerManager.Init();
if (SaveManager::GetInstance().toContinue &&
		!SaveManager::GetInstance().playerSaveData.preserveOnLoad)
	{
		PlayerManager::GetInstance().Load();
	}

	if (SaveManager::GetInstance().toContinue)
	{
		MapManager::GetInstance().LoadMapState();
		SaveManager::GetInstance().toContinue = false;
	}

	AddGameObjectToVector(playerManager.meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangedPlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangePlayerArrow, levelGameObjectVector);
	EnemyManager::GetInstance().Init(playerManager.currentPlayer);
	EnemyManager::GetInstance().SpawnEnemies(5, 1, levelGameObjectVector);

	InitGameObjects(levelGameObjectVector);
	
}

void ParkourLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	playerManager.Update();
	UpdateGameObjects(levelGameObjectVector);
	EnemyManager::GetInstance().UpdateAllEnemies(dt);
}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.6f, 0.8f, 0.85f);
	MapManager::GetInstance().DrawMapSprite();

	//player1->Render();
	playerManager.Render();
	EnemyManager::GetInstance().RenderEnemies();
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
	switch (next) {
	case GAME_STATE_TYPE::LEVEL1:
	case GAME_STATE_TYPE::LEVEL2:
		SaveManager::GetInstance().SetPreservePlayerOnLoad(false);
		break;
	case GAME_STATE_TYPE::LEVEL1BOSS:
	case GAME_STATE_TYPE::LEVEL2BOSS:
		SaveManager::GetInstance().SetPreservePlayerOnLoad(true);
		break;
	default:
		SaveManager::GetInstance().SetPreservePlayerOnLoad(false);
		break;
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
