// ---------------------------------------------------------------------------
// includes
#include <crtdbg.h> // To check for memory leaks
#include <vector>
#include <iostream>
#include "AEEngine.h"
#include "MapManager.h"
#include "PlayerGameObject.h"
#include "GameObjectManager.h"
//#include "TextComponent.h"
#include "GameStateManager.h"
#include "TextManager.h"
#include "PauseMenu.h"
#include "MainMenu.h"
#include "EnemyGameObject.h"
#include "EnemyManager.h"
#include "AudioManager.h"
#include "CameraSystem.h"
#include "LoadingScreen.h"

int gGameRunning = 1;

// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//_CrtSetBreakAlloc(136607);
	//int gGameRunning = 1;

	// Initialization of your own variables go here

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AudioManager::GetInstance().Init();

	// Changing the window title
	AESysSetWindowTitle("My New Demo!");


	printf("Hello LEVEL1\n");

	GameStateManager::GetInstance().Initialize(GAME_STATE_TYPE::SPLASH);
	CameraSystem::Init();

	// Game Loop
	while (gGameRunning)
	{
		// reset the system modules
		AESysReset();
		// Informing the system about the loop's start

		if (current != GAME_STATE_TYPE::RESTART)
		{
			GameStateManager::GetInstance().Update();

			// Initialize the current game state
			fpLoad();

		}
		else {
			next = current = previous;
		}

		fpInitialize();

		while (next == current && gGameRunning)
		{
			AESysFrameStart();
			// Update game logic for the current frame
			fpUpdate();
			double dt = AEFrameRateControllerGetFrameTime();
			CameraSystem::Update(dt);

			// Render graphics for the current frame
			fpRender();
			// check if forcing the application to quit
			if (AEInputCheckCurr(AEVK_RETURN)) {
				next = GAME_STATE_TYPE::RESTART;
			}
			if (0 == AESysDoesWindowExist())
				gGameRunning = 0;

			if (AEInputCheckCurr(AEVK_1))
				AESysSetFullScreen(1);
			if (AEInputCheckCurr(AEVK_2))
				AESysSetFullScreen(0);

			// to quickly switch between levels
			if (AEInputCheckCurr(AEVK_3)) {
				if (current != GAME_STATE_TYPE::LEVEL1) {
					LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL1;
					GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
				}
			}
			if (AEInputCheckCurr(AEVK_4)) {
				if (current != GAME_STATE_TYPE::LEVEL2) {
					LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL2;
					GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
				}
			}
			if (AEInputCheckCurr(AEVK_5)) {
				if (current != GAME_STATE_TYPE::LEVEL3) {
					LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL3;
					GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
				}
			}
			// Informing the system about the loop's end

			AESysFrameEnd();
		}
		fpFree();
		if (next != GAME_STATE_TYPE::RESTART)
		{
			fpUnload();	

		}
		previous = current;
		current = next;
	}
	AudioManager::GetInstance().Exit();
	// free the system
	AESysExit();
}