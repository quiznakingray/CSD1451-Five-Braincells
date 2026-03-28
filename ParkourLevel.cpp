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

//Player* player1 = nullptr;

std::vector<GameObject*> levelGameObjectVector{};

void ParkourLevel::Load()
{
	TextManager::Init();
	AudioManager::GetInstance().Init();
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

	EnemyManager::GetInstance().Init(PlayerManager::GetInstance().currentPlayer);
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
	HUD::GetInstance().Update(dt);
	EnemyManager::GetInstance().UpdateAllEnemies(dt);
}

void ParkourLevel::Render()
{
	AEGfxSetBackgroundColor(0.6f, 0.8f, 0.85f);
	MapManager::GetInstance().DrawMapSprite();

	PlayerManager::GetInstance().Render();
	EnemyManager::GetInstance().RenderEnemies();
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
	//FreeGameObjects(levelGameObjectVector);
	for (auto* obj : levelGameObjectVector) {
		// skip players � PlayerManager owns and deletes them
		if (obj == PlayerManager::GetInstance().meleePlayer) continue;
		if (obj == PlayerManager::GetInstance().rangedPlayer) continue;
		if (obj == PlayerManager::GetInstance().rangePlayerArrow) continue;
		if (dynamic_cast<Tile*>(obj)) continue;
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
