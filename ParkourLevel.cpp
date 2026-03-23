#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "GameStateManager.h"

//Player* player1 = nullptr;
EnemyGameObject* enemy1 = nullptr;

std::vector<GameObject*> gameObjects1{};

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

	if (SaveManager::GetInstance().toContinue)
	{
		MapManager::GetInstance().LoadMapState();
		SaveManager::GetInstance().toContinue = false;
	}
	//player->Init();
	//player1 = new Player();
	//playerManager.Load();
	//enemy1 = new EnemyGameObject();
	AddGameObjectToVector(PlayerManager::GetInstance().meleePlayer, gameObjects1);
	AddGameObjectToVector(PlayerManager::GetInstance().rangedPlayer, gameObjects1);
	AddGameObjectToVector(PlayerManager::GetInstance().rangePlayerArrow, gameObjects1);
	//AddGameObjectToVector(enemy1, gameObjects1);
	InitGameObjects(gameObjects1);
}

void ParkourLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	PlayerManager::GetInstance().Update();
	UpdateGameObjects(gameObjects1);
}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.6f, 0.8f, 0.85f);
	MapManager::GetInstance().DrawMapSprite();

	//player1->Render();
	PlayerManager::GetInstance().Render();
	//enemy1->Render();

}

void ParkourLevel::Free()
{
	//playerManager.Save();

	// Clean up game objects
	for (auto* obj : gameObjects1)
	{
		if (obj )
			delete obj;
	}
	gameObjects1.clear();
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
