// ---------------------------------------------------------------------------
// includes

#include <crtdbg.h> // To check for memory leaks
#include "AEEngine.h"
#include "MapManager.h"


int gGameRunning = 1;
AEGfxVertexList* pMesh = 0;
AEGfxTexture* pTex = 0;

#pragma region tempFuncs
// temporary functions
void RenderGraphics() {
	// Informing the system about the loop's start
	AESysFrameStart();

	// Your own rendering logic goes here
	// Set the background to black.
	AEGfxSetBackgroundColor(0.5f, 0.5f, 0.5f);

	
	DrawMap(0);

	


	// check if forcing the application to quit
	if (AEInputCheckCurr(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		gGameRunning = 0;

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

	InitMap("C:/Users/konxi/CSD1451-Five-Braincells/Maps/Map_Level_01.csv", 0);
	PrintMap(0);
}
void GameUpdate() {
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


	

	// Initialization of your own variables go here
	
	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);

	// Changing the window title
	AESysSetWindowTitle("My New Demo!");

	// reset the system modules
	AESysReset();

	printf("Hello World\n");

	GameInit();
	
	//InitMap("Map_Level_01.csv", 0);
	// Game Loop
	while (gGameRunning)
	{
		//// Informing the system about the loop's start
		//AESysFrameStart();

		//// Basic way to trigger exiting the application
		//// when ESCAPE is hit or when the window is closed
		//if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
		//	gGameRunning = 0;

		//// Your own update logic goes here
		GameUpdate();

		//// Your own rendering logic goes here
		//AEGfxMeshStart();
		//AEGfxTriAdd(
		//	-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
		//	0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		//	-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

		//AEGfxTriAdd(
		//	0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		//	0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		//	-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

		//// Saving the mesh (list of triangles) in pMesh
		//pMesh = AEGfxMeshEnd();

		//pTex = AEGfxTextureLoad("Assets/PlanetTexture.png");

		//// Create a scale matrix that scales by 500 x and y
		//AEMtx33 scale = { 0 };
		//AEMtx33Scale(&scale, 500.f, 500.f);

		//// Create a rotation matrix that rotates by 90 degrees
		//// Note that PI in radians is 180 degrees.
		//// Since 90 degrees is 180/2, 90 degrees in radians is PI/2
		//AEMtx33 rotate = { 0 };
		//AEMtx33Rot(&rotate, PI / 2);

		//// Create a translation matrix that translates by
		//// 200 in the x-axis and 100 in the y-axis
		//AEMtx33 translate = { 0 };
		//AEMtx33Trans(&translate, 200.f, 100.f);

		//// Concatenate the matrices into the 'transform' variable.
		//// We concatenate in the order of translation * rotation * scale
		//// i.e. this means we scale, then rotate, then translate.
		//AEMtx33 transform = { 0 };
		//AEMtx33Concat(&transform, &rotate, &scale);
		//AEMtx33Concat(&transform, &translate, &transform);



		//// Tell the Alpha Engine to set the background to black.
		//AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);

		//// Tell the Alpha Engine to get ready to draw something with texture.
		//AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

		//// Set the the color to multiply to white, so that the sprite can 
		//// display the full range of colors (default is black).
		//AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

		//// Set the color to add to nothing, so that we don't alter the sprite's color
		//AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

		//// Set blend mode to AE_GFX_BM_BLEND, which will allow transparency.
		//AEGfxSetBlendMode(AE_GFX_BM_BLEND);
		//AEGfxSetTransparency(1.0f);

		//// Tell Alpha Engine to use the texture stored in pTex
		//AEGfxTextureSet(pTex, 0, 0);

		//// Tell Alpha Engine to use the matrix in 'transform' to apply onto all 
		//// the vertices of the mesh that we are about to choose to draw in the next line.
		//AEGfxSetTransform(transform.m);

		//// Tell Alpha Engine to draw the mesh with the above settings.
		//AEGfxMeshDraw(pMesh, AE_GFX_MDM_TRIANGLES);
		//
		//

		//// Informing the system about the loop's end
		//AESysFrameEnd();

	}

	//AEGfxMeshFree(pMesh);
	//AEGfxTextureUnload(pTex);
	// free the system
	AESysExit();
}
