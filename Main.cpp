// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include <vector>
#include <iostream>
#include "AEEngine.h"
#include "MapManager.h"
#include "SpriteManager.h"
#include "CollisionManager.h"
#include "GameObjectManager.h"
#include "PlayerGameObject.h"


int gGameRunning = 1;
AEGfxVertexList* pMesh = 0;
AEGfxTexture* pTex = 0;

std::vector<Sprite*> spriteArr{};
Player* player{};
GameObject* player1{};
std::vector<GameObject* > go{};

#pragma region tempFuncs
// temporary functions
void RenderGraphics() {
	// Informing the system about the loop's start
	AESysFrameStart();

	// Your own rendering logic goes here
	// Set the background to black.
	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);
	DrawMapSprite(0);

	// check if forcing the application to quit
	if (AEInputCheckCurr(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gGameRunning = 0;

	//RenderSpriteArray(spriteArr);
	RenderGameObjects(go);

	if (AEInputCheckCurr(AEVK_1))
		AESysSetFullScreen(1);
	if (AEInputCheckCurr(AEVK_2))
		AESysSetFullScreen(0);

	// Informing the system about the loop's end
	AESysFrameEnd();
}
void GameInit()
{
	s32 windowWidth = AEGfxGetWindowWidth();
	s32 windowHeight = AEGfxGetWindowHeight();
	// Clears game background
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

	//InitMap("C:/Users/konxi/CSD1451-Five-Braincells/Maps/Map_Level_01.csv", 0);
	//PrintMap(0);

	//player = new GameObject(100.f,100.f, 0.f, 0.f, 1.f);
	//player->AddComponent(
	//	new Sprite(100.f, 100.f, 0.f, 0.f, 0.f,0.f, 0xFFFFFF00));

	//Collider* c = player->AddComponent(
	//	new Collider(COLLIDER_TYPE::BOX_COLLIDER, 25)
	//);
	//player->showColliders = true;
	player = new Player();
	//AddGameObjectToVector(player, go);
	go.push_back(player);
	player1 = new GameObject(100.f,100.f, 150.f, 100.f);
	player1->AddComponent(
		new Sprite(100.f, 100.f, 150.f, 100.f, 0.f,0.f, 0xFF0000FF));

	player1->AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, -25)
	);
	
	player1->showColliders = true;
	AddGameObjectToVector(player1, go);
	//AddSpriteToArray(spriteArr, player);

	for (GameObject* g : go)
	{
		g->Init();
	}
	InitMap("Assets/Maps/Map_Level_01.csv", 0);
	PrintMap(0);
}
void GameUpdate() {
	RenderGraphics();
	//UpdateSpriteArray(spriteArr);
	//player->Update();
	UpdateGameObjects(go);
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
		GameUpdate();
	}
	//FreeMap();
	// free the system
	//FreeSprite(player);
	AESysExit();
}
