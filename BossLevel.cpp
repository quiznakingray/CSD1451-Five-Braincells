#include "BossLevel.h"
#include "GameObjectManager.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include "EnemyGameObject.h"

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
}

void BossLevel::Update()
{
	double dt = AEFrameRateControllerGetFrameTime();
	UpdateGameObjects(gameObjects2);
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
}

void BossLevel::Unload()
{

}
