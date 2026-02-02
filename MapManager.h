#pragma once
#include "AEEngine.h"
#include "SpriteManager.h"
#include "TileData.h"
#include <rapidcsv.h>
#include "SingletonTemplate.h"
#include "GameObjectManager.h"

#define MAX_XAXIS 100
#define MAX_YAXIS 100
#define MAX_LEVELS 5

enum class TILE_ID {
	EMPTY = 0,
	GROUND = 100,
	SPIKEDOWN = 101,
	SPIKEUP = 102,
	SPIKELEFT = 103,
	SPIKERIGHT = 104,
	WALL = 110,
	LEVERREDOFF = 120,
	LEVERREDON = 121,
	LEVERGREENOFF = 122,
	LEVERGREENON = 123,
	LASERRED = 130,
	LASERGREEN = 131,
	PLAYER = 200,
	GOAL = 300,
};
//using TILE_ID = enum TILE_ID;

struct Tile : GameObject {
	Sprite * currSprite{};
	Sprite * bgSprite{};
	TILE_ID currID{};
	TILE_ID bgID{};
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

	const float tileSize = 37.5f;

	// Checks if tile needs to have special properties applied
	void CheckTileToInit(Tile* tile);

	Tile(TILE_ID a, TILE_ID b, int c, bool d, bool e, AEGfxTexture* currTex, AEGfxTexture* bgTex, size_t f, size_t g) :
		currID{ a }, bgID{ b }, currTag{ c }, ogTag{ c }, isBGActive{ d }, isCurrActive{ e }, currSprite{}, bgSprite{}, 
		row { f }, col{ g }
	{
		if (bgTex) {
			bgSprite = AddComponent(new Sprite());
			bgSprite->texture = bgTex;
		}
		currSprite = AddComponent(new Sprite());
		currSprite->texture = currTex;

		AEVec2Set(&scale, tileSize, tileSize);
		f32 x = -((f32)AEGfxGetWindowWidth() * 0.5f) + ((scale.x) * (col + 1));
		f32 y = ((f32)AEGfxGetWindowHeight() * 0.5f) - ((scale.y) * (row + 1));
		AEVec2Set(&pos, x, y);
		CheckTileToInit(this);
	};
	void Update() override;
};
//using Tile = struct Tile;

struct GroundTile : Tile {
	GroundTile(TILE_ID a, TILE_ID b, int c, bool d, bool e, AEGfxTexture* currTex, AEGfxTexture* bgTex, size_t f, size_t g) :
		Tile{ a, b, c, d, e, currTex, bgTex, f, g } {
		
	};
	~GroundTile() {
	};
};

struct WallTile : Tile {
	WallTile(TILE_ID a, TILE_ID b, int c, bool d, bool e, AEGfxTexture* currTex, AEGfxTexture* bgTex, size_t f, size_t g) :
		Tile{ a, b, c, d, e, currTex, bgTex, f, g } {
		
	};
	~WallTile() {
	};
};

static std::array<TILE_ID,4> spikes = {TILE_ID::SPIKEDOWN , TILE_ID::SPIKEUP, TILE_ID::SPIKELEFT, TILE_ID::SPIKERIGHT};

//template <typename S>
struct MapManager {


	const char delimiter = ',';
	static int mapCurrLevel;

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
	Tile* InitTile(int mapIndex, std::string cell, size_t col, size_t row);

	// Sets tile variables
	AEGfxTexture* SetTileTexture(TILE_ID currID);
	


	// rotates tile based on given rotation
	void RotateTile(double rotation, Tile tile);

	// Resets tile currID back to bgID
	void ResetTile(unsigned int col, unsigned int row);

	// Returns true if tile currID is on map regardless of position
	bool FindTile(unsigned int* col, unsigned int* row, unsigned int currID);

	// Gets tile from col and row provided
	Tile* GetTile(unsigned int col, unsigned int row);

	// Sets map info
	void SetTile(unsigned int col, unsigned int row, unsigned int currID, unsigned int currTag);

	// Finds all tiles on the map with provided currTag
	std::vector<Tile*> GetTaggedTiles(int tag);

	// Finds all tiles on the map with provided currTag and id
	std::vector<Tile*> GetTaggedTiles(int tag, TILE_ID id);

	// compares row of tiles in ascending order
	int compRowAsc(const Tile** t1, const Tile** t2);

	// compares rows of tiles in descending order
	int compRowDsc(const Tile** t1, const Tile** t2);

	// compares cols of tiles in ascending order
	int compColAsc(const Tile** t1, const Tile** t2);

	// compares cols of tiles in descending order
	int compColDsc(const Tile** t1, const Tile** t2);

#pragma endregion

#pragma region LaserFuncs
	void SetLaserActive(Tile tile, bool active);
#pragma endregion

#pragma region GetFuncs
	static AEVec2 GetPlayerSpawnPos();

	void AddTilesToGameObjectVector(std::vector<GameObject*>& gos);
};






