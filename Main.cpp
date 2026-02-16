// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "MapManager.h"
#include "PlayerGameObject.h"
#include "GameObjectManager.h"
#include <filesystem>


int gGameRunning = 1;
AEGfxVertexList* pMesh = 0;
AEGfxTexture* pTex = 0;

MapManager mapManager;

Player* player = new Player();

std::vector<GameObject*> gameObjects{};
#pragma region tempFuncs
// temporary functions
void RenderGraphics() {

	// Your own rendering logic goes here
	// Set the background to black.
	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	mapManager.DrawMapSprite();

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

	mapManager.InitMap("Assets/Maps/Map_Level_01.csv", 0);
	mapManager.PrintMap();

	mapManager.AddTilesToGameObjectVector(gameObjects);

	player->Init();
	AddGameObjectToVector(player, gameObjects);
	
	
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

	GameInit();

	// Game Loop
	while (gGameRunning)
	{
		// Informing the system about the loop's start
		AESysFrameStart();

		GameUpdate();

		// Informing the system about the loop's end
		AESysFrameEnd();
	}
	mapManager.FreeMap();
	// free the system
	AESysExit();
}
