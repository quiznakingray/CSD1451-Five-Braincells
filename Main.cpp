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
#include "MainMenu.h"
#include <filesystem>
#include "EnemyGameObject.h"
#include "EnemyManager.h"

int gGameRunning = 1;
//AEGfxVertexList* pMesh = 0;
//AEGfxTexture* pTex = 0;

MapManager mapManager;
TextManager textManager;
s8 TextManager::pFont = 0;


//std::vector<GameObject*> gameObjects{};

GameStateManager gameStateManager;
#pragma region tempFuncs
// temporary functions
void RenderGraphics() {

	// Your own rendering logic goes here
	// Set the background to black.
	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager.DrawMapSprite();

	// check if forcing the application to quit
	if (AEInputCheckCurr(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gGameRunning = 0;

	if (AEInputCheckCurr(AEVK_1))
		AESysSetFullScreen(1);
	if (AEInputCheckCurr(AEVK_2))
		AESysSetFullScreen(0);
}
void GameUpdate() {
	double dt = AEFrameRateControllerGetFrameTime();
	//UpdateGameObjects(gameObjects);
	RenderGraphics();
}
#pragma endregion

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


	//int gGameRunning = 1;

	// Initialization of your own variables go here

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

	// Changing the window title
	AESysSetWindowTitle("My New Demo!");

	// reset the system modules
	AESysReset();

	printf("Hello LEVEL1\n");

	gameStateManager.Initialize(GAME_STATE_TYPE::MENU);

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		gameStateManager.Update();

		//// Initialize the current game state
		fpLoad();
		fpInitialize();
		while (next == current && gGameRunning)
		{
			AESysFrameStart();
			// Update game logic for the current frame
			fpUpdate();
			// Render graphics for the current frame
			fpRender();
			// check if forcing the application to quit
			if (AEInputCheckCurr(AEVK_ESCAPE)) {
				next = GAME_STATE_TYPE::MENU;
			}
			if (0 == AESysDoesWindowExist())
				gGameRunning = 0;

			if (AEInputCheckCurr(AEVK_1))
				AESysSetFullScreen(1);
			if (AEInputCheckCurr(AEVK_2))
				AESysSetFullScreen(0);

			if (AEInputCheckCurr(AEVK_3)) {
				if (mapManager.mapCurrLevel != 1) {
					gameStateManager.ChangeState(GAME_STATE_TYPE::LEVEL2);
					gameStateManager.Update();
					mapManager.ChangeMap(1);
				}

			}
			if (AEInputCheckCurr(AEVK_4)) {
				if (mapManager.mapCurrLevel != 0) {
					gameStateManager.ChangeState(GAME_STATE_TYPE::LEVEL1);
					gameStateManager.Update();
					mapManager.ChangeMap(0);
				}
			}
			// Informing the system about the loop's end

			AESysFrameEnd();
		}
		//GameUpdate();

		fpFree();
		fpUnload();
		current = next;
	}
	//mapManager.FreeMap();
	//AEGfxDestroyFont(textManager.pFont);
	// free the system
	AESysExit();
}