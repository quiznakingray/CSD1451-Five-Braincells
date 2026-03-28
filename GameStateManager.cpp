#include "GameStateManager.h"
#include "ParkourLevel.h"
#include "BossLevel.h"
#include "MainMenu.h"


FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpRender = nullptr, fpFree = nullptr, fpUnload = nullptr;

GAME_STATE_TYPE current = GAME_STATE_TYPE::LEVEL1, next = GAME_STATE_TYPE::LEVEL2;

void GameStateManager::Initialize(GAME_STATE_TYPE type)
{
	current = next = type;
}

void GameStateManager::ChangeState(GAME_STATE_TYPE type)
{
	next = type;
}

GAME_STATE_TYPE GameStateManager::GetCurrentState() {
	return current;
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
	case GAME_STATE_TYPE::LEVEL1:  
	case GAME_STATE_TYPE::LEVEL1BOSS:
	case GAME_STATE_TYPE::LEVEL2:
	case GAME_STATE_TYPE::LEVEL2BOSS:
		fpLoad = ParkourLevel::Load;  
		fpInitialize = ParkourLevel::Init;  
		fpUpdate = ParkourLevel::Update;  
		fpRender = ParkourLevel::Render;  
		fpFree = ParkourLevel::Free;  
		fpUnload = ParkourLevel::Unload;  
		break;
	default:
		break;
	}
}
