#ifndef MAP_MANAGER_H 
#define MAP_MANAGER_H

#include "AEEngine.h"
#include "SpriteManager.h"
#include "TileData.h"
#include <rapidcsv.h>
#include <functional>
#include <iostream>
#include <algorithm>
#include "SingletonTemplate.h"
#include "PlayerGameObject.h"
#include "GameObjectManager.h"
#include "TextComponent.h"
#include "InputManager.h"
#include "GameStateManager.h"

#include "Helper.h"

#define MAX_XAXIS 100
#define MAX_YAXIS 100
#define MAX_LEVELS 5

struct PlayerManager; // forward declaration

enum class TILE_ID {
	EMPTY = 0,
	GRASSCENTER = 100,
	SPIKEDOWN = 101,
	SPIKEUP = 102,
	SPIKELEFT = 103,
	SPIKERIGHT = 104,
	CLOUD = 105,
	NOCOLLISIONGROUND = 106,
	WALL = 110,
	GRASSLEFT = 111,
	GRASSRIGHT = 112,
	GRASSTOP = 113,
	GRASSMID = 114,
	DIRTLEFT = 115,
	DIRTRIGHT = 116,
	DIRTTOP = 117,
	DIRTMID = 118,
	DIRTCENTER = 119,
	LEVERREDOFF = 120,
	LEVERREDON = 121,
	LEVERGREENOFF = 122,
	LEVERGREENON = 123,
	LEVERBLUEOFF = 124,
	LEVERBLUEON = 125,
	LASERRED = 130,
	LASERGREEN = 131,
	LASERBLUE = 132,
	CRATE = 140,
	BUTTONBLUEUNPRESSED = 150,
	BUTTONBLUEPRESSED = 151,
	BUTTONBLUETIMEDUNPRESSED = 152,
	BUTTONBLUETIMEDPRESSED = 153,
	GATE = 160,
	HEALTHPICKUPTILE = 170,
	PLAYER = 200,
	ENEMY = 250,
	CHECKPOINT = 300,
	GOAL = 500,
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
		size_t row_ = 0,
		size_t col_ = 0,
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
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {

		currSprite->textureFileName = "Assets/Environment/spike.png";
	}

	void Init() override {
		Tile::Init();
		collider->size.x = 0.7f; 
		collider->size.y = 0.7f;
		collider->OnCollisionEnter = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				player->health--;
				std::cout << player->health << '\n';
				// knockback based on collision side
				RigidBody* playerRb = player->GetComponent<RigidBody>();
				float knockbackX = 1000.0f;
				float knockbackY = 500.0f;

				switch (static_cast<int>(currID))
				{
					case static_cast<int>(TILE_ID::SPIKEUP):
						playerRb->velocity.y = -knockbackY;
						break;
					case static_cast<int>(TILE_ID::SPIKEDOWN):
						playerRb->velocity.y = knockbackY;
						break;
					case static_cast<int>(TILE_ID::SPIKELEFT):
						playerRb->velocity.x = knockbackX;
						break;
					case static_cast<int>(TILE_ID::SPIKERIGHT):
						playerRb->velocity.x = -knockbackX;
						break;
					default:
						break;
				}
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
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {

		switch (currID_) {
		case TILE_ID::NOCOLLISIONGROUND:
		case TILE_ID::GRASSCENTER:
			currSprite->textureFileName = "Assets/Environment/GRASSCENTER.png";
			break;
		case TILE_ID::GRASSLEFT:
			currSprite->textureFileName = "Assets/Environment/grassLeft.png";
			break;
		case TILE_ID::GRASSRIGHT:
			currSprite->textureFileName = "Assets/Environment/grassRight.png";
			break;
		case TILE_ID::GRASSTOP:
			currSprite->textureFileName = "Assets/Environment/grassTop.png";
			break;
		case TILE_ID::GRASSMID:
			currSprite->textureFileName = "Assets/Environment/grassMid.png";
			break;
		case TILE_ID::DIRTCENTER:
			currSprite->textureFileName = "Assets/Environment/dirtCenter.png";
			break;
		case TILE_ID::DIRTLEFT:
			currSprite->textureFileName = "Assets/Environment/dirtLeft.png";
			break;
		case TILE_ID::DIRTRIGHT:
			currSprite->textureFileName = "Assets/Environment/dirtRight.png";
			break;
		case TILE_ID::DIRTTOP:
			currSprite->textureFileName = "Assets/Environment/dirtTop.png";
			break;
		case TILE_ID::DIRTMID:
			currSprite->textureFileName = "Assets/Environment/dirtMid.png";
			break;
		}
		

	}

	void Init() override {
		Tile::Init();
		if (currID == TILE_ID::NOCOLLISIONGROUND) {
			collider->isTrigger = true;
			collider->OnTriggerEnter = [this](Collider* other, int sides) {
				if (Player* player = dynamic_cast<Player*>(other->owner))
				{
					currSprite->opacity = 0.2f;
				}
				};
			collider->OnTriggerExit = [this](Collider* other, int sides) {
				if (Player* player = dynamic_cast<Player*>(other->owner))
				{
					currSprite->opacity = 1.f;
				}
				};
		}
	}
};

struct WallTile : Tile {
	WallTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {


		currSprite->textureFileName = "Assets/Environment/wall.png";

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
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {

		if (currID == TILE_ID::LASERRED)
			currSprite->textureFileName = "Assets/Environment/laserRedVertical.png";
		else if (currID == TILE_ID::LASERGREEN)
			currSprite->textureFileName = "Assets/Environment/laserGreenVertical.png";
		else
			currSprite->textureFileName = "Assets/Environment/laserBlueVertical.png";

	}

	void Init() override {
		Tile::Init();
		collider->size.x = 0.4f;
		isActive = isCurrActive;
		collider->canCollide = isCurrActive;
	}


};

struct CrateTile : Tile {
	bool playerTouching = false;
	bool playerOnLeft = false;
	bool playerOnRight = false;

	Player* grabbedPlayer = nullptr;
	bool pushState = false;
	float pushForce = 50.0f;
	float friction = 40.0f;
	RigidBody* rb = nullptr;
	int grabbedSide = 0;
	CrateTile(
		TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, false, true) {

		currSprite->textureFileName = "Assets/Environment/crate.png";
		rb = AddComponent(
			new RigidBody()
		);
		
		rb->type = RIGIDBODY_TYPE::DYNAMIC;
		rb->mass = 5000.f;
		rb->invMass = 1.0f / rb->mass;
		rb->maxImpulse = 500000.f;
		rb->maxSpeed = 300.f;
		rb->hasGravity = true;
	}

	void Init() override;
	void Update() override;
};

struct CheckpointTile : Tile {
	CheckpointTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true) {
		currSprite->textureFileName = "Assets/Environment/checkpoint.png";
	}
	void Init() override;
};

struct HealthPickupTile : Tile {
	HealthPickupTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true) {
		currSprite->textureFileName = "Assets/Environment/gemRed.png";
	}
	void Init() override {
		Tile::Init();
		collider->isTrigger = true;
		collider->OnTriggerEnter = [this](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);
			if (player)
			{
				isCurrActive = false;
			}
			};
		
	}
};

struct CloudTile : Tile {
	double maxTimer = 3.0;
	double currTimer = 0.0;
	bool hasPlayerStepped = false;
	CooldownTimer cloudTimer;
	CooldownTimer respawnTimer;
	bool isDisappeared = false;
	RigidBody* rb = nullptr;

	CloudTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {
		rb = AddComponent(
			new RigidBody()
		);
		rb->type = RIGIDBODY_TYPE::STATIC;
		rb->hasGravity = false;
		currSprite->textureFileName = "Assets/Environment/cloud.png";
	}
	void StartCloudCountdown()
	{
		cloudTimer.Start(2.0);
	}


	void Init() override {
		Tile::Init();
		collider->OnCollisionEnter = [this](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);
			CrateTile* crate = dynamic_cast<CrateTile*>(other->owner);
			if (player || crate)
			{
				float cloudTop = pos.y + scale.y * 0.5f;
				float objectBottom = other->owner->pos.y - other->owner->scale.y * 0.5f;
				if (!hasPlayerStepped && objectBottom >= cloudTop - 5.f)
				{
					hasPlayerStepped = true;
					if (!cloudTimer.IsActive())
						cloudTimer.Start(2.0f);
				}
			}
			};

		collider->OnCollisionOver = [this](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);
			CrateTile* crate = dynamic_cast<CrateTile*>(other->owner);
			if (player || crate)
			{
				float cloudTop = pos.y + scale.y * 0.5f;
				float objectBottom = other->owner->pos.y - other->owner->scale.y * 0.5f;

				if (!hasPlayerStepped && objectBottom >= cloudTop - 5.f)
				{
					hasPlayerStepped = true;
					if (!cloudTimer.IsActive())
						cloudTimer.Start(2.0);
				}
			}
		};
	}

	void Update() override
	{
		Tile::Update();
		f32 dt = static_cast<f32>(AEFrameRateControllerGetFrameTime());

		if (!isDisappeared)
		{
			if (cloudTimer.Update(dt)){
				isCurrActive = false;
				collider->canCollide = false;
				isDisappeared = true;
				hasPlayerStepped = false;
				currSprite->opacity = 0.0f;
				respawnTimer.Start(5.0f);
			}

			if (cloudTimer.IsActive())
			{
				f32 progress = cloudTimer.GetProgress();
				currSprite->opacity = 1.0f - progress;
			}
		}
		else
		{
			if (respawnTimer.Update(dt))
			{
				isCurrActive = true;
				collider->canCollide = true;
				isDisappeared = false;
				currSprite->opacity = 1.0f;
			}
		}
	}
};

struct GateTile : Tile {
	GateTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {
		currSprite->textureFileName = "Assets/Environment/gate.png";
	}
	void Init() override {
		Tile::Init();
		collider->size.x = 0.5f;
	}

};

struct GoalTile : Tile {
	GoalTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true) {
		currSprite->textureFileName = "Assets/Environment/doorclose.png";
	}
	void Init() override;
};

struct MapManager : public Singleton<MapManager> {

	static constexpr  float tileSize = 80.f;
	const char delimiter = ',';
	static unsigned int mapCurrLevel;
	static size_t rowCount;
	static size_t colCount;

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

	void SaveMapState(GAME_STATE_TYPE level);

	void LoadMapState();
	
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

	// returns a vector of all tiles with given currID
	std::vector<Tile*> GetTilesWithID(TILE_ID currID);

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
	static Tile* GetTile(TILE_ID id);
	static AEVec2 GetPlayerSpawnPos();

	void AddTilesToGameObjectVector(std::vector<GameObject*>& gos);
};


struct LeverTile : Tile {
	int altTag = 0;
	LeverTile(
		TILE_ID currID_ = TILE_ID::EMPTY,
		TILE_ID bgID_ = TILE_ID::EMPTY,
		int currTag_ = 0,
		int altTag_ = 0,
		bool bgActive = false,
		bool currActive = true,
		size_t row_ = 0,
		size_t col_ = 0,
		float tileSize = 0.f)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true) {
		altTag = altTag_;
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
		case TILE_ID::LEVERBLUEON:
			currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserBlueSwitchOn.png");
			break;
		case TILE_ID::LEVERBLUEOFF:
			currSprite->texture = AEGfxTextureLoad("Assets/Environment/laserBlueSwitchOff.png");
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
		case TILE_ID::LEVERBLUEON:
			currID = TILE_ID::LEVERBLUEOFF;
			break;
		case TILE_ID::LEVERBLUEOFF:
			currID = TILE_ID::LEVERBLUEON;
			break;
		default:
			currID = TILE_ID::LEVERREDON;
			break;
		}

		SetTexture();
	}

	void ActivateLasers(bool activate, TILE_ID tileId) {
		for (Tile* laser : MapManager::GetTaggedTiles(currTag, tileId))
		{
			laser->isActive = activate;
			laser->isCurrActive = activate;
			if (laser->collider) laser->collider->canCollide = activate;
		}
		for (Tile* laser : MapManager::GetTaggedTiles(altTag, tileId))
		{
			laser->isActive = !activate;
			laser->isCurrActive = !activate;
			if (laser->collider) laser->collider->canCollide = !activate;
		}
	}

	void TriggerLever(Arrow* arrow = nullptr)
	{
		TILE_ID tileId;
		if (currID == TILE_ID::LEVERREDON || currID == TILE_ID::LEVERREDOFF)
			tileId = TILE_ID::LASERRED;
		else if (currID == TILE_ID::LEVERGREENON || currID == TILE_ID::LEVERGREENOFF)
			tileId = TILE_ID::LASERGREEN;
		else
			tileId = TILE_ID::LASERBLUE;

		this->ToggleLever();

		std::vector<Tile*> lasers = MapManager::GetTaggedTiles(currTag, tileId);
		bool currentlyActive = !lasers.empty() && lasers[0]->isActive;
		ActivateLasers(!currentlyActive, tileId);

		if (arrow)
		{
			arrow->isActive = false;
			arrow->timer = 0.0f;
		}
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
			if (Arrow* arrow = dynamic_cast<Arrow*>(other->owner))
			{
				if (!arrow->isActive) return;
				TriggerLever(arrow);
			}
			};
		collider->OnTriggerOver = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				this->interactionTextBox->isActive = true;
				if (AEInputCheckTriggered(AEVK_F))
				{
					TriggerLever();
				}
			}
			// catch arrows that spawned inside the trigger
			if (Arrow* arrow = dynamic_cast<Arrow*>(other->owner))
			{
				if (!arrow->isActive) return;
				TriggerLever(arrow);
			}
			};
		collider->OnTriggerExit = [this](Collider* other, int sides) {
			if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				this->interactionTextBox->isActive = false;
			}
			};

		interactionTextBox->text = "[F] Pull";
	}
};

struct ButtonTile : Tile {
	bool isPressed = false;
	int playerCount = 0;
	int crateCount = 0;
	bool isTimed = false;
	int altTag = 0;

	CooldownTimer gateTimer;
	f32 gateInterval = 0.5;
	std::vector<Tile*> gateQueue;
	bool activate = false;
	bool queueRunning = false;

	bool hasPendingActivation = false;
	bool pendingActivate = false;
	bool pendingPressedDown = false;
	bool gatesAreActive = true;
	bool gatesOriginalState = false;
	ButtonTile(
		TILE_ID currID_ = TILE_ID::EMPTY,
		TILE_ID bgID_ = TILE_ID::EMPTY,
		int currTag_ = 0,
		int altTag_ = 0,
		bool bgActive = false,
		bool currActive = true,
		size_t row_ = 0,
		size_t col_ = 0,
		float tileSize = 0.f,
		bool isTimed_ = false)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {
		altTag = altTag_;
		isTimed = isTimed_;
		SetTexture();
	}

	void SetTexture() {
		switch (currID)
		{
		case TILE_ID::BUTTONBLUEUNPRESSED:
		case TILE_ID::BUTTONBLUETIMEDUNPRESSED:
			currSprite->texture = AEGfxTextureLoad("Assets/Environment/buttonBlueUnpressed.png");
			break;
		case TILE_ID::BUTTONBLUEPRESSED:
		case TILE_ID::BUTTONBLUETIMEDPRESSED:
			currSprite->texture = AEGfxTextureLoad("Assets/Environment/buttonBluePressed.png");
			break;
		default:
			currSprite->texture = AEGfxTextureLoad("Assets/Environment/buttonBlueUnpressed.png");
			break;
		}
	}

	void ToggleButton()
	{
		switch (currID)
		{
		case TILE_ID::BUTTONBLUEPRESSED:
		case TILE_ID::BUTTONBLUETIMEDPRESSED:
			currID = isTimed ? TILE_ID::BUTTONBLUETIMEDUNPRESSED : TILE_ID::BUTTONBLUEUNPRESSED;
			break;
		case TILE_ID::BUTTONBLUEUNPRESSED:
		case TILE_ID::BUTTONBLUETIMEDUNPRESSED:
			currID = isTimed ? TILE_ID::BUTTONBLUETIMEDPRESSED : TILE_ID::BUTTONBLUEPRESSED;
			break;
		default:
			currID = TILE_ID::BUTTONBLUEUNPRESSED;
			break;
		}
		SetTexture();
	}

	void ActivateGates(bool _activate) {
		if (!isTimed) {
			for (Tile* gate : MapManager::GetTaggedTiles(currTag, TILE_ID::GATE))
			{
				gate->isActive = _activate;
				gate->isCurrActive = _activate;
				if (gate->collider) gate->collider->canCollide = _activate;
			}
			for (Tile* gate : MapManager::GetTaggedTiles(altTag, TILE_ID::GATE))
			{
				gate->isActive = !_activate;
				gate->isCurrActive = !_activate;
				if (gate->collider) gate->collider->canCollide = !_activate;
			}
		}
		else {
			gateTimer.Stop();
			queueRunning = false;
			gateQueue.clear();

			// get all gates
			std::vector<Tile*> allGates = MapManager::GetTaggedTiles(currTag, TILE_ID::GATE);

			// filter first - only keep gates that haven't reached target state
			for (Tile* gate : allGates)
			{
				if (gate->isActive != _activate)
					gateQueue.push_back(gate);
			}

			if (_activate) {
				// activating: bottom to top
				std::sort(gateQueue.begin(), gateQueue.end(),
					[](Tile* a, Tile* b) { return a->row > b->row; });
			}
			else {
				// deactivating: top to bottom
				std::sort(gateQueue.begin(), gateQueue.end(),
					[](Tile* a, Tile* b) { return a->row < b->row; });
			}

			activate = _activate;
			queueRunning = !gateQueue.empty();
			if (queueRunning)
				gateTimer.Start(gateInterval);
		}
	}

	void Update() override
	{
		Tile::Update();

		if (!queueRunning || gateQueue.empty())
		{
			if (hasPendingActivation)
			{
				hasPendingActivation = false;
				gatesAreActive = pendingActivate;
				ActivateGates(pendingActivate);
			}
			return;
		}

		f32 dt = static_cast<f32>(AEFrameRateControllerGetFrameTime());

		if (gateTimer.Update(dt))
		{
			Tile* gate = gateQueue.front();
			gateQueue.erase(gateQueue.begin());

			if (gate)
			{
				gate->isActive = activate;
				gate->isCurrActive = activate;
				if (gate->collider) gate->collider->canCollide = activate;
			}

			if (!gateQueue.empty())
				gateTimer.Start(gateInterval);
			else
				queueRunning = false;
		}
	}

	void Init() override {
		Tile::Init();
		collider->center.y = 0.f;
		collider->size.x = 0.9f;
		collider->size.y = 0.5f;
		collider->isTrigger = true;

		std::vector<Tile*> gates = MapManager::GetTaggedTiles(currTag, TILE_ID::GATE);
		gatesOriginalState = !gates.empty() && gates[0]->isActive;
		gatesAreActive = gatesOriginalState;

		collider->OnTriggerEnter = [this](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);
			CrateTile* crate = dynamic_cast<CrateTile*>(other->owner);
			bool wasPressed = isPressed;

			if (player) playerCount++;
			if (crate)  crateCount++;

			isPressed = (playerCount + crateCount) > 0;

			if (!wasPressed && isPressed) {
				ToggleButton();
				bool targetState = !gatesOriginalState;  // pressed = opposite of original
				if (queueRunning) {
					hasPendingActivation = true;
					pendingActivate = targetState;
				}
				else {
					gatesAreActive = targetState;
					ActivateGates(targetState);
				}
			}
		};

		collider->OnTriggerExit = [this](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);
			CrateTile* crate = dynamic_cast<CrateTile*>(other->owner);
			bool wasPressed = isPressed;

			if (player) playerCount = max(0, playerCount - 1);
			if (crate)  crateCount = max(0, crateCount - 1);

			isPressed = (playerCount + crateCount) > 0;

			if (wasPressed && !isPressed) {
				ToggleButton();
				// released = return to original state
				if (queueRunning) {
					hasPendingActivation = true;
					pendingActivate = gatesOriginalState;
				}
				else {
					gatesAreActive = gatesOriginalState;
					ActivateGates(gatesOriginalState);
				}
			}
		};
	}
};
#endif // !MAP_MANAGER