//---------------------------------------------------------
// author:    Kon Xin Yi & Ang Zi Jun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "GameStateManager.h"
#include "ParkourLevel.h"
#include "MainMenu.h"
#include "PauseMenu.h"
#include "LoadingScreen.h"
#include "SplashScreen.h"
#include "Credits.h"
#include "Controls.h"


FP fpLoad = nullptr, fpInitialize = nullptr, fpUpdate = nullptr, fpRender = nullptr, fpFree = nullptr, fpUnload = nullptr;

GAME_STATE_TYPE current = GAME_STATE_TYPE::MENU, next = GAME_STATE_TYPE::MENU, previous = GAME_STATE_TYPE::MENU;

void GameStateManager::Initialize(GAME_STATE_TYPE type)
{
	current = next = type;
}

void GameStateManager::ChangeState(GAME_STATE_TYPE type)
{
	next = type;

	if (type == GAME_STATE_TYPE::MENU)
	{
		AudioManager::GetInstance().PlayMusic("mainMenu");
	}
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
	case GAME_STATE_TYPE::SPLASH:
		fpLoad = SplashScreen::Load;
		fpInitialize = SplashScreen::Init;
		fpUpdate = SplashScreen::Update;
		fpRender = SplashScreen::Render;
		fpFree = SplashScreen::Free;
		fpUnload = SplashScreen::Unload;
		break;
	case GAME_STATE_TYPE::MENU:
		fpLoad = []() {}; // Empty or load background textures
		fpInitialize = MainMenu_Init;
		fpUpdate = MainMenu_Update;
		fpRender = MainMenu_Draw;
		fpFree = MainMenu_Free;
		fpUnload = []() {};
		break;
	case GAME_STATE_TYPE::LOADING:
		fpLoad = LoadingScreen::Load;
		fpInitialize = LoadingScreen::Init;
		fpUpdate = LoadingScreen::Update;
		fpRender = LoadingScreen::Render;
		fpFree = LoadingScreen::Free;
		fpUnload = LoadingScreen::Unload;
		break;
	case GAME_STATE_TYPE::LEVEL1:  
	case GAME_STATE_TYPE::LEVEL2:
	case GAME_STATE_TYPE::LEVEL3:
		fpLoad = ParkourLevel::Load;  
		fpInitialize = ParkourLevel::Init;  
		fpUpdate = ParkourLevel::Update;  
		fpRender = ParkourLevel::Render;  
		fpFree = ParkourLevel::Free;  
		fpUnload = ParkourLevel::Unload;  
		break;
;
	case GAME_STATE_TYPE::CREDITS:
		fpLoad = []() {}; // Nothing to load/unload from disk
		fpInitialize = CreditsMenu::Credits_Init;
		fpUpdate = CreditsMenu::Credits_Update;
		fpRender = CreditsMenu::Credits_Draw;
		fpFree = CreditsMenu::Credits_Free;
		fpUnload = []() {};
		break;


	case GAME_STATE_TYPE::CONTROLS:
		fpLoad = []() {};
		fpInitialize = ControlsMenu::Controls_Init;
		fpUpdate = ControlsMenu::Controls_Update;
		fpRender = ControlsMenu::Controls_Draw;
		fpFree = ControlsMenu::Controls_Free;
		fpUnload = []() {};
		break;
	default:
		break;
	}
}
