#include "ParkourLevel.h"
#include "GameObjectManager.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include "EnemyGameObject.h"

Player* player1 = nullptr;
EnemyGameObject* enemy = nullptr;

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
	mapManager1.PrintMap(0);

	mapManager1.AddTilesToGameObjectVector(gameObjects1);

	//player->Init();
	player1 = new Player();
	enemy = new EnemyGameObject();
	AddGameObjectToVector(player1, gameObjects1);
	AddGameObjectToVector(enemy, gameObjects1);

	InitGameObjects(gameObjects1);
}

void ParkourLevel::Update()
{
	UpdateGameObjects(gameObjects1);
}

void ParkourLevel::Render()
{

	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager1.DrawMapSprite(0);

	player1->Render();
	enemy->Render();

}

void ParkourLevel::Free()
{
	// Clean up game objects
	for (auto* obj : gameObjects1)
	{
		if (obj && obj != player1)
			delete obj;
	}
	gameObjects1.clear();

	// Clean up player1
	if (player1)
	{
		delete player1;
		player1 = nullptr;
	}

	mapManager1.FreeMap();
	AEGfxSetCamPosition(0.f, 0.f);
}

void ParkourLevel::Unload()
{

}
