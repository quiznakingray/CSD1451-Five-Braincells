// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "MapManager.h"
#include "PlayerGameObject.h"
#include "GameObjectManager.h"
//#include "TextComponent.h"
#include "GameStateManager.h"
#include <filesystem>


int gGameRunning = 1;
//AEGfxVertexList* pMesh = 0;
//AEGfxTexture* pTex = 0;

MapManager mapManager;
TextManager textManager;
s8 TextManager::pFont = 0;

Player* player = new Player();

std::vector<GameObject*> gameObjects{};

GameStateManager gameStateManager;
#pragma region tempFuncs
// temporary functions
void RenderGraphics() {

	// Your own rendering logic goes here
	// Set the background to black.
	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager.DrawMapSprite(0);

	player->Render();

	// check if forcing the application to quit
	if (AEInputCheckCurr(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gGameRunning = 0;

	if (AEInputCheckCurr(AEVK_1))
		AESysSetFullScreen(1);
	if (AEInputCheckCurr(AEVK_2))
		AESysSetFullScreen(0);
}
void GameInit()
{
	s32 windowWidth = AEGfxGetWindowWidth();
	s32 windowHeight = AEGfxGetWindowHeight();
	// Clears game background
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	//mapManager.GetInstance();
	TextManager::Init();

	mapManager.InitMap("Assets/Maps/Map_Level_01.csv", 0);
	mapManager.PrintMap(0);

	mapManager.AddTilesToGameObjectVector(gameObjects);

	//player->Init();
	AddGameObjectToVector(player, gameObjects);
	
	
	InitGameObjects(gameObjects);
}
void GameUpdate() {
	UpdateGameObjects(gameObjects);
	RenderGraphics();
	//player->Update();
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

	// Initialization of your own variables go here
	
	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

	// Changing the window title
	AESysSetWindowTitle("My New Demo!");

	// reset the system modules
	AESysReset();

	printf("Hello World\n");

	//GameInit();
	gameStateManager.Initialize(GAME_STATE_TYPE::WORLD);

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();
		gameStateManager.Update();

		//// Initialize the current game state
		fpLoad();
		fpInitialize();
		while (next == current)
		{
			// Update game logic for the current frame
			fpUpdate();
			// Render graphics for the current frame
			fpRender();
			// check if forcing the application to quit
			if (AEInputCheckCurr(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
				gGameRunning = 0;

			if (AEInputCheckCurr(AEVK_1))
				AESysSetFullScreen(1);
			if (AEInputCheckCurr(AEVK_2))
				AESysSetFullScreen(0);
			// Informing the system about the loop's end
			AESysFrameEnd();
			AESysFrameStart();

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
