#ifndef MAP_MANAGER_H 
#define MAP_MANAGER_H

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
#include "InputManager.h"
#include "Helper.h"

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
	CLOUD = 105,
	WALL = 110,
	LEVERREDOFF = 120,
	LEVERREDON = 121,
	LEVERGREENOFF = 122,
	LEVERGREENON = 123,
	LASERRED = 130,
	LASERGREEN = 131,
	CRATE = 140,
	PLAYER = 200,
	ENEMY = 250,
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

	Tile(
		TILE_ID curr_ID = TILE_ID::EMPTY,
		TILE_ID bg_ID = TILE_ID::EMPTY,
		int currTag_ = 0,
		bool bgActive = false,
		bool currActive = true,
		int row_ = 0, 
		int col_ = 0,
		float tileSize = 0.f,
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
		, isTrigger(is_Trigger)
		, canInteract(can_Interact)
	{
		// scale
		AEVec2Set(&scale, tileSize, tileSize);

		// position
		f32 x = static_cast<f32>(- AEGfxGetWindowWidth() * 0.5f + scale.x * (col + 1));
		f32 y = static_cast<f32>(AEGfxGetWindowHeight() * 0.5f - scale.y * (row + 1));
		AEVec2Set(&pos, x, y);

		currSprite = AddComponent(
			new Sprite()
		);

		//showColliders = true;
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
		collider->OnTriggerOver = [](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				std::cout << "In spike" << std::endl;
			}
			};
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
struct LaserTile : Tile {
	LaserTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {


		currSprite->texture = currID == TILE_ID::LASERRED ?
			AEGfxTextureLoad("Assets/Environment/laserRedVertical.png")
			:
			AEGfxTextureLoad("Assets/Environment/laserGreenVertical.png");

	}

	void Init() override {
		Tile::Init();
		collider->size.x = 0.4f;
		//showColliders = true;
	}
};


struct CloudTile : Tile {
	double maxTimer = 3.0;
	double currTimer = 0.0;
	bool hasPlayerStepped = false;
	CooldownTimer cloudTimer;

	CloudTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {


		currSprite->texture =  AEGfxTextureLoad("Assets/Environment/images.png");
	}
	void StartCloudCountdown()
	{
		cloudTimer.Start(2.0);
	}


	void Init() override {
		Tile::Init();
		collider->OnCollisionEnter = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				// Check player's X center is within this tile's X bounds
				// Use a small tolerance to handle floating point edge cases
				float tolerance = scale.x * 0.5f;
				bool playerWithinXBounds = other->owner->pos.x >= (pos.x - scale.x * 0.5f - tolerance) &&
					other->owner->pos.x <= (pos.x + scale.x * 0.5f + tolerance);

				if (!hasPlayerStepped && (sides & COLLISION_SIDE::TOP) && playerWithinXBounds)
				{
					hasPlayerStepped = true;
					if (!cloudTimer.IsActive())
						cloudTimer.Start(2.0);
				}
			}
		};

		collider->OnCollisionOver = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				float tolerance = scale.x * 0.5f;
				bool playerWithinXBounds = other->owner->pos.x >= (pos.x - scale.x * 0.5f - tolerance) &&
					other->owner->pos.x <= (pos.x + scale.x * 0.5f + tolerance);

				if (!hasPlayerStepped && (sides & COLLISION_SIDE::TOP) && playerWithinXBounds)
				{
					hasPlayerStepped = true;
					if (!cloudTimer.IsActive())
						cloudTimer.Start(2.0);
				}
			}
		};
		collider->OnCollisionExit = [this](Collider* other, int sides) {
		};
	}

	void Update() override
	{
		Tile::Update();

		double dt = AEFrameRateControllerGetFrameTime();

		if (cloudTimer.Update(dt))
		{
			isActive = false;
			collider->canCollide = false;
		}

		if (cloudTimer.IsActive())
		{
			float progress = cloudTimer.GetProgress();
			currSprite->opacity = (f32)(1.0f - progress);
		}
	}
};

struct CrateTile : Tile {
	bool playerTouching = false;
	bool playerOnLeft = false;
	bool playerOnRight = false;
	float pushForce = 50.0f;
	float friction = 40.0f;
	RigidBody* rb = nullptr;
	CrateTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		int row_,
		int col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {

		currSprite->texture = AEGfxTextureLoad("Assets/Environment/crate.png");
		rb = AddComponent(
			new RigidBody()
		);

		rb->type = RIGIDBODY_TYPE::DYNAMIC;
		rb->mass = 10.f;
		rb->invMass = 1.0f / rb->mass;
		rb->maxImpulse = 50.f;
		rb->maxSpeed = 75.f;
	}

	void Init() override;
	void Update() override;
};

//template <typename S>
struct MapManager : public Singleton<MapManager> {

	static constexpr  float tileSize = 80.f;
	const char delimiter = ',';
	static unsigned int mapCurrLevel;
	static unsigned int rowCount;
	static unsigned int colCount;

	static MapManager* mapManager;
	//S MapManager() {};
	//S ~MapManager() {};
	//S get_instance();


#pragma region MapFuncs
	// Loads a map
	void InitMap(std::string fileName, unsigned int currLevel);

	void ChangeMap(unsigned int currLevel);

	void PrintMap();

	void LoopMap(void* (mapfunc)());

	void DrawMapSprite();

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
	static std::vector<Tile*> GetTaggedTiles(int tag, TILE_ID id);

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
	static Tile * GetTile(TILE_ID id);
	static AEVec2 GetPlayerSpawnPos();

	void AddTilesToGameObjectVector(std::vector<GameObject*>& gos);
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

		SetTexture();
		
	}

	void SetTexture() {
		switch (currID)
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

	void ToggleLever()
	{
		switch (currID)
		{
			case TILE_ID::LEVERREDON:
				currID = TILE_ID::LEVERREDOFF;
				break;
			case TILE_ID::LEVERREDOFF:
				currID = TILE_ID::LEVERREDON;
				break;
			case TILE_ID::LEVERGREENON:
				currID = TILE_ID::LEVERGREENOFF;
				break;
			case TILE_ID::LEVERGREENOFF:
				currID = TILE_ID::LEVERGREENON;
				break;
			default:
				currID = TILE_ID::LEVERREDON;
				break;
		}

		SetTexture();

	}
	void Init() override {
		Tile::Init();

		//showColliders = true;

		collider->center.y = 0.5f;
		collider->size.x = 2.f;
		collider->size.y = 1.5f;
		collider->isTrigger = true;
		collider->OnTriggerEnter = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				this->interactionTextBox->isActive = true;
			}
			};
		collider->OnTriggerOver = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				this->interactionTextBox->isActive = true;
				if (AEInputCheckTriggered(AEVK_F))
				{
					this->ToggleLever();

					TILE_ID tileId = currID == TILE_ID::LEVERREDOFF || currID == TILE_ID::LEVERREDON
						? TILE_ID::LASERRED : TILE_ID::LASERGREEN;

					std::vector<Tile*> taggedTiles = MapManager::GetTaggedTiles(currTag, tileId);

					for (Tile* laser : taggedTiles)
					{
						laser->isActive = (currID == TILE_ID::LEVERREDON || currID == TILE_ID::LEVERGREENON);
					}
				}
			}
			};
		collider->OnTriggerExit = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				this->interactionTextBox->isActive = false;
			}
			};

		interactionTextBox->text = "[F]";

	}

};
#endif // !MAP_MANAGER 

