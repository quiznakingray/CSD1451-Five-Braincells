#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "EnemyGameObject.h"
#include "PlayerManager.h"
#include "EnemyManager.h"
#include "CameraSystem.h"
#include "InputManager.h"
#include "TextManager.h"
#include "HUD.h"
#include "PauseMenu.h"
#include "EndMenu.h"
#include "FireGameObject.h"
#include "ParticleEffects.h"


std::vector<GameObject*> levelGameObjectVector{};
ParticleSystem fireRainOverlay;

void ParkourLevel::Load()
{
	//TextManager::Init();
	//AudioManager::GetInstance().Init();
}

void ParkourLevel::Init()
{
	// Clears game background
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	switch (current) {
	case GAME_STATE_TYPE::LEVEL1:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01.csv", GAME_STATE_TYPE::LEVEL1);
		break;
	case GAME_STATE_TYPE::LEVEL2:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_02.csv", GAME_STATE_TYPE::LEVEL2);
		break;
	case GAME_STATE_TYPE::LEVEL3:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_03.csv", GAME_STATE_TYPE::LEVEL3);
		break;
	default:
		MapManager::GetInstance().InitMap("Assets/Maps/Map_Level_01.csv", GAME_STATE_TYPE::LEVEL1);
		break;
	}
	
	MapManager::GetInstance().AddTilesToGameObjectVector(levelGameObjectVector);

	PlayerManager::GetInstance().Init();
	AddGameObjectToVector(PlayerManager::GetInstance().meleePlayer, levelGameObjectVector);
	AddGameObjectToVector(PlayerManager::GetInstance().rangedPlayer, levelGameObjectVector);
	for (Arrow* a : PlayerManager::GetInstance().arrowGameObjectPool)
	{
		AddGameObjectToVector(a, levelGameObjectVector);
	}
	//AddGameObjectToVector(PlayerManager::GetInstance().rangePlayerArrow, levelGameObjectVector);

	EnemyManager::GetInstance().Init(PlayerManager::GetInstance().meleePlayer, PlayerManager::GetInstance().rangedPlayer);
	EnemyManager::GetInstance().SpawnEnemies(levelGameObjectVector);

	SpawnFire(levelGameObjectVector);

	InitGameObjects(levelGameObjectVector);


	EndMenu::GetInstance().Init();
	


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

	// Pause
	PauseMenu::GetInstance().Init();

	fireRainOverlay.Init(30);
}

void ParkourLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	InputManager::GetInstance().Update();

	HUD::GetInstance().Update(dt);
	if (GameStateManager::GetInstance().gamePaused) {
		if (GameStateManager::GetInstance().showPauseMenu) {

			PauseMenu::GetInstance().Update();
			
		}
		EndMenu::GetInstance().Update();
		return;
	}
	
	fireRainOverlay.CreateFireRain(AEGfxGetWindowWidth(), AEGfxGetWindowHeight());
	fireRainOverlay.Update(dt);

	PlayerManager::GetInstance().Update();
	UpdateGameObjects(levelGameObjectVector);
}

void ParkourLevel::Render()
{
	AEGfxSetBackgroundColor(0.6f, 0.8f, 0.85f);
	MapManager::GetInstance().DrawMapSprite();
	RenderFire();
	PlayerManager::GetInstance().Render();
	EnemyManager::GetInstance().RenderEnemies();
	fireRainOverlay.Draw();
	HUD::GetInstance().Render();
	EndMenu::GetInstance().Render();
	if (GameStateManager::GetInstance().showPauseMenu) {
		PauseMenu::GetInstance().Render();
	}
	//if (EndMenu::GetInstance().isActive) {
		//EndMenu::GetInstance().Render();
	//}
}

void ParkourLevel::Free()
{
	// Clean up game objects
	//AEGfxSetCamPosition(0.f, 0.f);
	//FreeGameObjects(levelGameObjectVector);
	for (auto* obj : levelGameObjectVector) {
		// skip players PlayerManager owns and deletes them
		if (obj == PlayerManager::GetInstance().meleePlayer) continue;
		if (obj == PlayerManager::GetInstance().rangedPlayer) continue;
		//bool isArrow = false;
		if (Arrow* tile = dynamic_cast<Arrow*>(obj)) continue;
		if (EnemyGameObject* tile = dynamic_cast<EnemyGameObject*>(obj)) continue;
		//for (int i = 0; i < PlayerManager::GetInstance().arrowGameObjectPool.size(); i++)
		//{
		//	if (obj == PlayerManager::GetInstance().arrowGameObjectPool[i])
		//	{
		//		isArrow = true;
		//		break;
		//	}
		//}
		//if (isArrow) continue;


		if (dynamic_cast<Tile*>(obj)) continue;
		obj->Free();
		delete obj;
	}
	levelGameObjectVector.clear();
	MapManager::GetInstance().FreeMap();
	fireRainOverlay.Exit();
	HUD::GetInstance().Free();
	PlayerManager::GetInstance().Free();
	EnemyManager::GetInstance().FreeEnemies();
	FreeFire();
	CameraSystem::ResetCameraPosition();
	//for (auto* obj : levelGameObjectVector)
	//{
	//	obj = nullptr;
	//}
	PauseMenu::GetInstance().Free();
	EndMenu::GetInstance().Free();

	//EndMenu::GetInstance().Free();
	//EndMenu::GetInstance().isActive = false; // ResetAll for the next time the level loads
}

void ParkourLevel::Unload()
{
	GameStateManager::GetInstance().showPauseMenu = false;
}
