#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "CameraSystem.h"
#include "InputManager.h"
#include "HUD.h"


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

	AddGameObjectToVector(PlayerManager::GetInstance().meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(PlayerManager::GetInstance().rangedPlayer, levelGameObjectVector);
	AddGameObjectToVector(PlayerManager::GetInstance().rangePlayerArrow, levelGameObjectVector);
	EnemyManager::Init(PlayerManager::GetInstance().currentPlayer);
	EnemyManager::SpawnEnemies(5, 1, levelGameObjectVector);

	InitGameObjects(levelGameObjectVector);
	
	// HUD
	HUD::GetInstance().Init();
}

void ParkourLevel::Update()
{
	InputManager::GetInstance().Update();
	if (GameStateManager::GetInstance().isGamePause) return;
	double dt = AEFrameRateControllerGetFrameTime();
	PlayerManager::GetInstance().Update();
	UpdateGameObjects(levelGameObjectVector);
	EnemyManager::UpdateAllEnemies(dt);
	HUD::GetInstance().Update(dt);
}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.6f, 0.8f, 0.85f);
	MapManager::GetInstance().DrawMapSprite();

	//player1->Render();
	PlayerManager::GetInstance().Render();
	EnemyManager::RenderEnemies();
	HUD::GetInstance().Render();
}

void ParkourLevel::Free()
{
	//playerManager.Save();

	// Clean up game objects
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

	//AEGfxSetCamPosition(0.f, 0.f);
	FreeGameObjects(levelGameObjectVector);
	for (auto* obj : levelGameObjectVector) {
		// skip players — PlayerManager owns and deletes them
		if (obj == PlayerManager::GetInstance().meleePlayer) continue;
		if (obj == PlayerManager::GetInstance().rangedPlayer) continue;
		if (obj == PlayerManager::GetInstance().rangePlayerArrow) continue;

		obj->Free();
		delete obj;
	}
	levelGameObjectVector.clear();
	MapManager::GetInstance().FreeMap();
	HUD::GetInstance().Free();
	PlayerManager::GetInstance().Free();
	CameraSystem::ResetCameraPosition();
}

void ParkourLevel::Unload()
{

}
