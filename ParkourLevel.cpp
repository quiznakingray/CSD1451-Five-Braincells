#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "EnemyManager.h"

//Player* player1 = nullptr;

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
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01.csv", GAME_STATE_TYPE::LEVEL1);
		break;
	case GAME_STATE_TYPE::LEVEL1BOSS:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01b.csv", GAME_STATE_TYPE::LEVEL1BOSS);
		break;
	case GAME_STATE_TYPE::LEVEL2:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_02.csv", GAME_STATE_TYPE::LEVEL2);
		break;
	case GAME_STATE_TYPE::LEVEL2BOSS:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_02b.csv", GAME_STATE_TYPE::LEVEL2BOSS);
		break;
	default:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01.csv", GAME_STATE_TYPE::LEVEL3);
		break;
	}
	
	MapManager::GetInstance().AddTilesToGameObjectVector(levelGameObjectVector);

	PlayerManager::GetInstance().Init();
	AddGameObjectToVector(PlayerManager::GetInstance().meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(PlayerManager::GetInstance().rangedPlayer, levelGameObjectVector);
	AddGameObjectToVector(PlayerManager::GetInstance().rangePlayerArrow, levelGameObjectVector);

	AddGameObjectToVector(playerManager.meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangedPlayer, levelGameObjectVector);
	AddGameObjectToVector(playerManager.rangePlayerArrow, levelGameObjectVector);
	EnemyManager::GetInstance().Init(playerManager.currentPlayer);
	EnemyManager::GetInstance().SpawnEnemies(5, 1, levelGameObjectVector);

	InitGameObjects(levelGameObjectVector);

	if (SaveManager::GetInstance().toContinue)
	{
		// always load player when continuing from main menu
		PlayerManager::GetInstance().Load();

		MapManager::GetInstance().LoadMapState();
		EnemyManager::GetInstance().LoadEnemyStates();
		SaveManager::GetInstance().toContinue = false;
	}
}

void ParkourLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	PlayerManager::GetInstance().Update();
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
