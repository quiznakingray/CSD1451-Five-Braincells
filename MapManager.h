#pragma once
#include "AEEngine.h"
#include "SpriteManager.h"
#include "TileData.h"
#include <rapidcsv.h>
#include "SingletonTemplate.h"

#define MAX_XAXIS 100
#define MAX_YAXIS 100
#define MAX_LEVELS 5

struct Tile {
	Sprite currSprite{};
	Sprite bgSprite{};
	int currID{};
	int bgID{};
	int currTag{};
	int ogTag{};
	size_t row{};
	size_t col{};
	bool isTrigger{};
	bool isCollidable{};
	bool isCenter{};
	bool isCurrActive{ true };
	bool isBGActive{ true };
	union {
		Spike spike{};
	};
};
using Tile = struct Tile;

enum TILE_ID {
	EMPTY = 0,
	GROUND = 100,
	SPIKE = 101,
	WALL = 102,
	PLAYER = 200,
	GOAL = 300,
};
using TILE_ID = enum TILE_ID;

//template <typename S>
struct MapManager {

	const float tileSize = 37.5f;
	const char delimiter = ',';
	int mapCurrLevel;

	static MapManager* mapManager;
	//S MapManager() {};
	//S ~MapManager() {};
	//S get_instance();

#pragma region MapFuncs
	// Loads a map
	void InitMap(std::string fileName, unsigned int currLevel);

	void PrintMap(unsigned int currLevel);

	void LoopMap(void* (mapfunc)());

	void DrawMapSprite(int currLevel);

	void DrawMapCollision(int currLevel);

	void FreeMap();
#pragma endregion

#pragma region TileFuncs
	// Draws a tile on screen
	void DrawTile(Sprite currSprite, AEMtx33 transform);

	// Inits tile variables
	Tile InitTile(int mapIndex, std::string cell, size_t col, size_t row);

	// Sets tile variables
	AEGfxTexture* SetTileTexture(unsigned int currID);

	// Resets tile currID back to bgID
	void ResetTile(unsigned int col, unsigned int row);

	// Returns true if tile currID is on map regardless of position
	bool FindTile(unsigned int* col, unsigned int* row, unsigned int currID);

	// Gets tile from col and row provided
	Tile* GetTile(unsigned int col, unsigned int row);

	// Sets map info
	void SetTile(unsigned int col, unsigned int row, unsigned int currID, unsigned int currTag);

	// Finds all tiles on the map with provided currTag
	Tile** GetTaggedTiles(int size, Tile* tile);

	// compares row of tiles in ascending order
	int compRowAsc(const Tile** t1, const Tile** t2);

	// compares rows of tiles in descending order
	int compRowDsc(const Tile** t1, const Tile** t2);

	// compares cols of tiles in ascending order
	int compColAsc(const Tile** t1, const Tile** t2);

	// compares cols of tiles in descending order
	int compColDsc(const Tile** t1, const Tile** t2);

#pragma endregion
};





