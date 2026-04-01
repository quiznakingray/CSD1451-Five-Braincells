#include "GameStateManager.h"
#include "ParkourLevel.h"
#include "MainMenu.h"
#include "PauseMenu.h"


FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpRender = nullptr, fpFree = nullptr, fpUnload = nullptr;

GAME_STATE_TYPE current = GAME_STATE_TYPE::MENU, next = GAME_STATE_TYPE::MENU, previous = GAME_STATE_TYPE::MENU;

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
	if (current == GAME_STATE_TYPE::RESTART)
		return;

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
;
	//case GAME_STATE_TYPE::PAUSE:
	//	fpLoad = []() {}; // Nothing to load/unload from disk
	//	fpInitialize = PauseMenu::Init;
	//	fpUpdate = PauseMenu::Update;
	//	fpRender = PauseMenu::Render;
	//	fpFree = PauseMenu::Free;
	//	fpUnload = []() {};
	//	break;

	//case GAME_STATE_TYPE::CONFIRMATION:
	//	fpLoad = []() {};
	//	fpInitialize = []() {}; // We reuse the mesh from PauseMenu::Init
	//	fpUpdate = ConfirmationMenu::Update;
	//	fpRender = ConfirmationMenu::Render;
	//	fpFree = []() {};     // PauseMenu::Free handles the mesh cleanup
	//	fpUnload = []() {};
	//	break;
	//case GAME_STATE_TYPE::SETTING:
	//	fpLoad = []() {};
	//	fpInitialize = Setting_Init; 
	//	fpUpdate = Setting_Update;
	//	fpRender = Setting_Draw;
	//	fpFree = Setting_Free;
	//	fpUnload = []() {};
	//	break;

	//case GAME_STATE_TYPE::INSTRUCTIONS:
	//	fpLoad = []() {};
	//	fpInitialize = Instructions_Init; 
	//	fpUpdate = Instructions_Update;
	//	fpRender = Instructions_Draw;
	//	fpFree = Instructions_Free;
	//	fpUnload = []() {};
	//	break;
	default:
		break;
	}
}
