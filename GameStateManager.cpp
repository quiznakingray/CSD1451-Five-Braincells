#include "GameStateManager.h"
#include "ParkourLevel.h"
#include "BossLevel.h"
#include "PlayerCombat.h"
#include "MainMenu.h"


FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpRender = nullptr, fpFree = nullptr, fpUnload = nullptr;

GAME_STATE_TYPE current = GAME_STATE_TYPE::WORLD, next = GAME_STATE_TYPE::OTHER;

void GameStateManager::Initialize(GAME_STATE_TYPE type)
{
	current = next = type;
}

void GameStateManager::ChangeState(GAME_STATE_TYPE type)
{
	next = type;
}

void GameStateManager::Update()
{
	switch (current)
	{
	case GAME_STATE_TYPE::MENU:
		fpLoad = []() {}; // Empty or load background textures
		fpInitialize = MainMenu_Init;
		fpUpdate = MainMenu_Update;
		fpRender = MainMenu_Draw;
		fpFree = MainMenu_Free;
		fpUnload = []() {};
		break;
	case GAME_STATE_TYPE::WORLD:  
		fpLoad = ParkourLevel::Load;  
		fpInitialize = ParkourLevel::Init;  
		fpUpdate = ParkourLevel::Update;  
		fpRender = ParkourLevel::Render;  
		fpFree = ParkourLevel::Free;  
		fpUnload = ParkourLevel::Unload;  
		break;
	case GAME_STATE_TYPE::OTHER:
		fpLoad = BossLevel::Load;
		fpInitialize = BossLevel::Init;
		fpUpdate = BossLevel::Update;
		fpRender = BossLevel::Render;
		fpFree = BossLevel::Free;
		fpUnload = BossLevel::Unload;
		break;
	case GAME_STATE_TYPE::COMBAT:  
		fpLoad = GameStateLoad;
		fpInitialize = GameStateInit;
		fpUpdate = GameStateUpdate;
		fpRender = GameStateDraw;
		fpFree = GameStateFree;
		fpUnload = GameStateUnload;
		break;
	default:
		break;
	}
}
