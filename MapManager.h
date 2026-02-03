#pragma once
#include "AEEngine.h"
#include "SpriteManager.h"
#include "TileData.h"
#include <rapidcsv.h>
#include <functional>
#include <iostream>
#include "SingletonTemplate.h"
#include "PlayerGameObject.h"
#include "GameObjectManager.h"
#include "TextComponent.h"

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
	Sprite* currSprite{};
	Sprite* bgSprite{};
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

	Collider* collider = nullptr;

	bool canInteract = false;
	Text* interactionTextBox = nullptr;

	union {
		Spike spike{};
	};

	Tile(
		TILE_ID curr_ID,
		TILE_ID bg_ID,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize,
		bool is_Trigger = false,
		bool can_Interact = false
	)
		: currID(curr_ID)
		, bgID(bg_ID)
		, currTag(currTag_)
		, ogTag(currTag_)
		, isBGActive(bgActive)
		, isCurrActive(currActive)
		, row(row_)
		, col(col_)
		,isTrigger(is_Trigger)
		,canInteract(can_Interact)
	{
		// scale
		AEVec2Set(&scale, tileSize, tileSize);

		// position
		f32 x = -AEGfxGetWindowWidth() * 0.5f + scale.x * (col + 1);
		f32 y = AEGfxGetWindowHeight() * 0.5f - scale.y * (row + 1);
		AEVec2Set(&pos, x, y);

		currSprite = AddComponent(
			new Sprite()
		);


	}


	void Init() override {

		collider = AddComponent(
			new Collider()
		);
		collider->isTrigger = isTrigger;


		if (canInteract)
		{
			interactionTextBox = AddComponent(
				new Text()
			);
			interactionTextBox->text = "[Interact]";
			interactionTextBox->center.y = 100.f;
			interactionTextBox->isActive = false;
		}

		GameObject::Init();
	}
	void Update() override;


};
//using Tile = struct Tile;


static std::array<TILE_ID, 4> spikes = { TILE_ID::SPIKEDOWN , TILE_ID::SPIKEUP, TILE_ID::SPIKELEFT, TILE_ID::SPIKERIGHT };

struct SpikeTile : Tile {

	SpikeTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true) {

		currSprite->texture = AEGfxTextureLoad("Assets/Environment/spike.png");
	}

	void Init() override {
		Tile::Init();
		collider->OnTriggerOver = [](Collider* other) {
			if (Player* tile = dynamic_cast<Player*>(other->owner))
			{
				std::cout << "In spike" << std::endl;
			}
		};
	}
};

struct LeverTile : Tile {

	LeverTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true) {

		switch (currID_)
		{
			case TILE_ID::LEVERREDON:
				currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOn.png");
				break;
			case TILE_ID::LEVERREDOFF:
				currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOff.png");
				break;
			case TILE_ID::LEVERGREENON:
				currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserGreenSwitchOn.png");
				break;
			case TILE_ID::LEVERGREENOFF:
				currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserGreenSwitchOff.png");
				break;
			default:
				currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOn.png");
				break;
		}
		
	}

	void Init() override {
		Tile::Init();

		showColliders = true;

		collider->isTrigger = true;

		//Collider* interactionBox = AddComponent(
		//	new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.f, 0.5f, 2.f, 1.5f)
		//);
		collider->center.y = 0.5f;
		collider->size.x = 2.f;
		collider->size.y = 1.5f;
		collider->isTrigger = true;
		collider->OnTriggerOver = [this](Collider* other) {
			if (Player* tile = dynamic_cast<Player*>(other->owner))
			{
				std::cout << "In lever" << std::endl;
				this->interactionTextBox->isActive= true;
			}
			};
		collider->OnTriggerExit = [this](Collider* other) {
			if (Player* tile = dynamic_cast<Player*>(other->owner))
			{
				//std::cout << "In lever" << std::endl;
				this->interactionTextBox->isActive= false;
			}
		};

		interactionTextBox->text = "[F]";

	}

};

struct GroundTile : Tile {
	GroundTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {


		currSprite->texture =  AEGfxTextureLoad("Assets/Environment/ground.png");

	}

	void Init() override {
		Tile::Init();
	}
};

struct WallTile : Tile {
	WallTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {


		currSprite->texture = AEGfxTextureLoad("Assets/Environment/wall.png");

	}

	void Init() override {
		Tile::Init();
	}
};
//template <typename S>
struct MapManager {

	static constexpr  float tileSize = 80.f;
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

	// Checks if tile needs to have special properties applied
	void CheckTileToInit(Tile* tile);

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

	// get tiles near position
	static std::vector<Tile*> GetTilesNearPos(AEVec2 pos, AEVec2 scale);
#pragma endregion

#pragma region LaserFuncs
	void SetLaserActive(Tile tile, bool active);
#pragma endregion

#pragma region GetFuncs
	static AEVec2 GetPlayerSpawnPos();

	void AddTilesToGameObjectVector(std::vector<GameObject*>& gos);
};






