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
#include "PlayerStats.h"
#include "AudioManager.h"
#include "PlayerManager.h"

#include "CooldownTimer.h"

#define MAX_XAXIS 100
#define MAX_YAXIS 100
#define MAX_LEVELS 5

struct PlayerManager; // forward declaration
struct EnemyManager; // forward declaration
struct PlayerStats; // forward declaration

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
	DAMAGEPICKUPTILE = 171,
	PROFPICKUPTILE = 172,
	MOVINGTILEMID = 180,
	MOVINGTILELEFT = 181,
	MOVINGTILERIGHT = 182,
	MOVINGTILETARGET = 183,
	MOVINGTILEBUTTONMID = 184,
	MOVINGTILEBUTTONLEFT = 185,
	MOVINGTILEBUTTONRIGHT = 186,
	SANDLEFT = 190,
	SANDRIGHT = 191,
	SANDTOP = 192,
	SANDMID = 193,
	SANDCENTER = 194,
	PLAYER = 200,
	ENEMYMELEE = 250,
	ENEMYRANGE = 251,
	MINIBOSSMELEE = 252,
	MINIBOSSRANGE = 253,
	CHECKPOINT = 300,
	GOAL = 500,
	// non interactables, only for decoration
	HOUSEBEIGEBOTTOMLEFT = 600,
	HOUSEBEIGEBOTTOMMID = 601,
	HOUSEBEIGEBOTTOMRIGHT = 602,
	HOUSEBEIGEMIDLEFT = 603,
	HOUSEBEIGEMID = 604,
	HOUSEBEIGEMIDRIGHT = 605,
	HOUSEBEIGETOPLEFT = 606,
	HOUSEBEIGETOPRIGHT = 607,

	HOUSEDARKBOTTOMLEFT = 608,
	HOUSEDARKBOTTOMMID = 609,
	HOUSEDARKBOTTOMRIGHT = 610,
	HOUSEDARKMIDLEFT = 611,
	HOUSEDARKMID = 612,
	HOUSEDARKMIDRIGHT = 613,
	HOUSEDARKTOPLEFT = 614,
	HOUSEDARKTOPRIGHT = 615,

	ROOFYELLOWMID = 616,
	ROOFYELLOWLEFT = 617,
	ROOFYELLOWRIGHT = 618,
	ROOFYELLOWTOPMID = 619,
	ROOFYELLOWTOPLEFT = 620,
	ROOFYELLOWTOPRIGHT = 621,

	ROOFREDMID = 622,
	ROOFREDLEFT = 623,
	ROOFREDRIGHT = 624,
	ROOFREDTOPMID = 625,
	ROOFREDTOPLEFT = 626,
	ROOFREDTOPRIGHT = 627,

	DOORKNOB = 628,
	DOORTOP = 629,
	WINDOW = 630,
	CHIMNEY = 631
};
//using TILE_ID = enum TILE_ID;

struct Tile : GameObject {
	Sprite* currSprite{};
	Sprite* bgSprite{};
	TILE_ID currID{};
	TILE_ID bgID{};
	int currTag{};
	int altTag{};
	size_t row{};
	size_t col{};
	bool isTrigger{};
	bool canCollide{};
	bool isCenter{};
	bool canArrowPass{ false };
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
		bool can_Interact = false,
		bool can_Collide = true
	)
		: currID(curr_ID)
		, bgID(bg_ID)
		, currTag(currTag_)
		, altTag(currTag_)
		, isBGActive(bgActive)
		, isCurrActive(currActive)
		, row(row_)
		, col(col_)
		, isTrigger(is_Trigger)
		, canInteract(can_Interact)
		, canCollide(can_Collide)
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

	~Tile();
	void Init() override {

		collider = AddComponent(
			new Collider()
		);
		collider->isTrigger = isTrigger;
		collider->canCollide = canCollide;
		if (canInteract)
		{
			interactionTextBox = AddComponent(
				new Text()
			);
			interactionTextBox->SetText("[Interact]");
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
		switch (currID) {
		case TILE_ID::SPIKEDOWN:
		{
			rotation = 0;
			break;
		}
		case TILE_ID::SPIKEUP:
		{
			rotation = PI;
			break;
		}
		case TILE_ID::SPIKELEFT:
		{
			rotation = 270 * (PI / 180);
			break;
		}
		case TILE_ID::SPIKERIGHT:
		{
			rotation = PI / 2;
			break;
		}
		}
		
	}
	int damage = 1;
	void Init() override;
};

struct HouseTile : Tile {
	HouseTile(
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
			// Beige House
		case TILE_ID::HOUSEBEIGEBOTTOMLEFT:
			currSprite->textureFileName = "Assets/Environment/houseBeigeBottomLeft.png";
			break;
		case TILE_ID::HOUSEBEIGEBOTTOMMID:
			currSprite->textureFileName = "Assets/Environment/houseBeigeBottomMid.png";
			break;
		case TILE_ID::HOUSEBEIGEBOTTOMRIGHT:
			currSprite->textureFileName = "Assets/Environment/houseBeigeBottomRight.png";
			break;
		case TILE_ID::HOUSEBEIGEMIDLEFT:
			currSprite->textureFileName = "Assets/Environment/houseBeigeMidLeft.png";
			break;
		case TILE_ID::HOUSEBEIGEMID:
			currSprite->textureFileName = "Assets/Environment/houseBeigeMid.png";
			break;
		case TILE_ID::HOUSEBEIGEMIDRIGHT:
			currSprite->textureFileName = "Assets/Environment/houseBeigeMidRight.png";
			break;
		case TILE_ID::HOUSEBEIGETOPLEFT:
			currSprite->textureFileName = "Assets/Environment/houseBeigeTopLeft.png";
			break;
		case TILE_ID::HOUSEBEIGETOPRIGHT:
			currSprite->textureFileName = "Assets/Environment/houseBeigeTopRight.png";
			break;

			// Dark House
		case TILE_ID::HOUSEDARKBOTTOMLEFT:
			currSprite->textureFileName = "Assets/Environment/houseDarkBottomLeft.png";
			break;
		case TILE_ID::HOUSEDARKBOTTOMMID:
			currSprite->textureFileName = "Assets/Environment/houseDarkBottomMid.png";
			break;
		case TILE_ID::HOUSEDARKBOTTOMRIGHT:
			currSprite->textureFileName = "Assets/Environment/houseDarkBottomRight.png";
			break;
		case TILE_ID::HOUSEDARKMIDLEFT:
			currSprite->textureFileName = "Assets/Environment/houseDarkMidLeft.png";
			break;
		case TILE_ID::HOUSEDARKMID:
			currSprite->textureFileName = "Assets/Environment/houseDarkMid.png";
			break;
		case TILE_ID::HOUSEDARKMIDRIGHT:
			currSprite->textureFileName = "Assets/Environment/houseDarkMidRight.png";
			break;
		case TILE_ID::HOUSEDARKTOPLEFT:
			currSprite->textureFileName = "Assets/Environment/houseDarkTopLeft.png";
			break;
		case TILE_ID::HOUSEDARKTOPRIGHT:
			currSprite->textureFileName = "Assets/Environment/houseDarkTopRight.png";
			break;

			// Yellow Roof
		case TILE_ID::ROOFYELLOWMID:
			currSprite->textureFileName = "Assets/Environment/roofYellowMid.png";
			break;
		case TILE_ID::ROOFYELLOWLEFT:
			currSprite->textureFileName = "Assets/Environment/roofYellowLeft.png";
			break;
		case TILE_ID::ROOFYELLOWRIGHT:
			currSprite->textureFileName = "Assets/Environment/roofYellowRight.png";
			break;
		case TILE_ID::ROOFYELLOWTOPMID:
			currSprite->textureFileName = "Assets/Environment/roofYellowTopMid.png";
			break;
		case TILE_ID::ROOFYELLOWTOPLEFT:
			currSprite->textureFileName = "Assets/Environment/roofYellowTopLeft.png";
			break;
		case TILE_ID::ROOFYELLOWTOPRIGHT:
			currSprite->textureFileName = "Assets/Environment/roofYellowTopRight.png";
			break;

			// Red Roof
		case TILE_ID::ROOFREDMID:
			currSprite->textureFileName = "Assets/Environment/roofRedMid.png";
			break;
		case TILE_ID::ROOFREDLEFT:
			currSprite->textureFileName = "Assets/Environment/roofRedLeft.png";
			break;
		case TILE_ID::ROOFREDRIGHT:
			currSprite->textureFileName = "Assets/Environment/roofRedRight.png";
			break;
		case TILE_ID::ROOFREDTOPMID:
			currSprite->textureFileName = "Assets/Environment/roofRedTopMid.png";
			break;
		case TILE_ID::ROOFREDTOPLEFT:
			currSprite->textureFileName = "Assets/Environment/roofRedTopLeft.png";
			break;
		case TILE_ID::ROOFREDTOPRIGHT:
			currSprite->textureFileName = "Assets/Environment/roofRedTopRight.png";
			break;

			// Props
		case TILE_ID::DOORKNOB:
			currSprite->textureFileName = "Assets/Environment/doorKnob.png";
			break;
		case TILE_ID::DOORTOP:
			currSprite->textureFileName = "Assets/Environment/doorTop.png";
			break;
		case TILE_ID::WINDOW:
			currSprite->textureFileName = "Assets/Environment/window.png";
			break;
		case TILE_ID::CHIMNEY:
			currSprite->textureFileName = "Assets/Environment/chimney.png";
			break;
		}
	}
	void Init() override {
		Tile::Init();
		collider->canCollide = false;
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
			switch (current) {
			case GAME_STATE_TYPE::LEVEL1:
				currSprite->textureFileName = "Assets/Environment/grassCenter.png";
				break;
			case GAME_STATE_TYPE::LEVEL2:
				currSprite->textureFileName = "Assets/Environment/dirtCenter.png";
				break;
			case GAME_STATE_TYPE::LEVEL3:
				currSprite->textureFileName = "Assets/Environment/sandCenter.png";
				break;
			default:
				currSprite->textureFileName = "Assets/Environment/grassCenter.png";
				break;
			}
			
		case TILE_ID::GRASSCENTER:
			currSprite->textureFileName = "Assets/Environment/grassCenter.png";
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
		case TILE_ID::SANDCENTER:
			currSprite->textureFileName = "Assets/Environment/sandCenter.png";
			break;
		case TILE_ID::SANDLEFT:
			currSprite->textureFileName = "Assets/Environment/sandLeft.png";
			break;
		case TILE_ID::SANDRIGHT:
			currSprite->textureFileName = "Assets/Environment/sandRight.png";
			break;
		case TILE_ID::SANDTOP:
			currSprite->textureFileName = "Assets/Environment/sandTop.png";
			break;
		case TILE_ID::SANDMID:
			currSprite->textureFileName = "Assets/Environment/sandMid.png";
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
		currSprite->textureFileName = "Assets/Environment/gemGreen.png";
	}
	void Init() override;
};

struct DamagePickupTile : Tile {
	int damageAmount = 1; // how much to add

	DamagePickupTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize,
		int damageAmount_ = 1)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true),
		damageAmount(damageAmount_)
	{
		currSprite->textureFileName = "Assets/Environment/gemRed.png";
	}

	void Init() override;
};

struct ProficiencyPickupTile : Tile {
	float proficiencyAmount = 0.1f; // amount to add (0..1)

	ProficiencyPickupTile(TILE_ID currID_,
		TILE_ID bgID_,
		int currTag_,
		bool bgActive,
		bool currActive,
		size_t row_,
		size_t col_,
		float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize, true, true)
	{
		currSprite->textureFileName = "Assets/Environment/gemBlue.png";
	}

	void Init() override;
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
		currSprite->textureFileName = "Assets/Environment/cloudLeft.png";
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

	static constexpr  float tileSize = 100.f;
	const char delimiter = ',';
	GAME_STATE_TYPE mapCurrLevel;
	static size_t rowCount;
	static size_t colCount;

	static MapManager* mapManager;
	//S MapManager() {};
	//S ~MapManager() {};
	//S get_instance();



#pragma region MapFuncs
	// Loads a map
	void InitMap(std::string fileName, GAME_STATE_TYPE currLevel);

	void ChangeMap(GAME_STATE_TYPE currLevel);

	void PrintMap();

	void LoopMap(void* (mapfunc)());

	void DrawMapSprite();

	void FreeMap();

	void GenerateNodes();

	void SaveMapState();

	void LoadMapState();
	
#pragma endregion

#pragma region TileFuncs
	// Draws a tile on screen
	void DrawTile(Sprite currSprite, AEMtx33 transform);

	// Inits tile variables
	Tile* InitTile(std::string cell, size_t col, size_t row);

	// Sets tile variables
	AEGfxTexture* SetTileTexture(TILE_ID currID);
	std::string GetTileTexture(TILE_ID currID);

	// Checks if tile needs to have special properties applied
	void CheckTileToInit(Tile* tile);

	// returns a vector of all tiles with given currID
	std::vector<Tile*> GetTilesWithID(TILE_ID currID);

	// Finds all tiles on the map with provided currTag
	std::vector<Tile*> GetTaggedTiles(int tag);

	// Finds all tiles on the map with provided currTag and id
	static std::vector<Tile*> GetTaggedTiles(int tag, TILE_ID id);

	static std::vector<Tile*> GetAltTaggedTiles(int altTag, TILE_ID id);

	// get tiles near position
	static std::vector<Tile*> GetTilesNearPos(AEVec2 pos, AEVec2 scale);
#pragma endregion

#pragma region LaserFuncs
	void SetLaserActive(Tile tile, bool active);
#pragma endregion

#pragma region GetFuncs
	// Gets tile from col and row provided
	Tile* GetTile(size_t col, size_t row);
	static Tile* GetTile(TILE_ID id);
	static AEVec2 GetPlayerSpawnPos();
	size_t GetRow();
	size_t GetCol();
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
			currSprite->textureFileName = "Assets/Environment/laserRedSwitchOn.png";
			std::cout << currSprite->textureFileName << '\n';
			break;
		case TILE_ID::LEVERREDOFF:
			currSprite->textureFileName = "Assets/Environment/laserRedSwitchOff.png";
			std::cout << currSprite->textureFileName << '\n';
			break;
		case TILE_ID::LEVERGREENON:
			currSprite->textureFileName = "Assets/Environment/laserGreenSwitchOn.png";
			break;
		case TILE_ID::LEVERGREENOFF:
			currSprite->textureFileName = "Assets/Environment/laserGreenSwitchOff.png";
			break;
		case TILE_ID::LEVERBLUEON:
			currSprite->textureFileName = "Assets/Environment/laserBlueSwitchOn.png";
			break;
		case TILE_ID::LEVERBLUEOFF:
			currSprite->textureFileName = "Assets/Environment/laserBlueSwitchOff.png";
			break;
		default:
			currSprite->textureFileName = "Assets/Environment/laserRedSwitchOn.png";
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
		AudioManager::GetInstance().PlaySFX("leverSwitch");
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
				// prevent triggering from melee shield collider
				if (AEInputCheckTriggered(AEVK_F) && !other->isTrigger &&
					PlayerManager::GetInstance().currentPlayer == player)
				{
					TriggerLever();
					std::cout << "lever triggered\n";
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

struct MovingTile : Tile {
	AEVec2 originPos{};
	AEVec2 targetPos{};
	float speed = 100.0f;
	bool movingToTarget = true;
	bool hasTarget = false;
	bool isWaiting = false;
	bool isEnabled = false;
	bool requiresButton = false;
	CooldownTimer waitTimer;
	float waitDuration = 2.0f;
	RigidBody* rb = nullptr;

	MovingTile(TILE_ID currID_, TILE_ID bgID_, int currTag_, int altTag_,
		bool bgActive, bool currActive, size_t row_, size_t col_, float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {
		switch (currID) {
		case TILE_ID::MOVINGTILEMID:
		case TILE_ID::MOVINGTILEBUTTONMID:
			currSprite->textureFileName = "Assets/Environment/shroomMid.png";
			break;
		case TILE_ID::MOVINGTILELEFT:
		case TILE_ID::MOVINGTILEBUTTONLEFT:
			currSprite->textureFileName = "Assets/Environment/shroomLeft.png";
			break;
		case TILE_ID::MOVINGTILERIGHT:
		case TILE_ID::MOVINGTILEBUTTONRIGHT:
			currSprite->textureFileName = "Assets/Environment/shroomRight.png";
			break;
		}
		altTag = altTag_;
		rb = AddComponent(new RigidBody());
		rb->type = RIGIDBODY_TYPE::KINEMATIC;
		rb->hasGravity = false;
		rb->invMass = 0.0f;

		if (currID == TILE_ID::MOVINGTILEBUTTONLEFT ||
			currID == TILE_ID::MOVINGTILEBUTTONMID ||
			currID == TILE_ID::MOVINGTILEBUTTONRIGHT) {
			requiresButton = true;
		}
		isEnabled = !requiresButton;
	}

	void Init() override {
		Tile::Init();
		originPos = pos;
		std::vector<Tile*> targets = MapManager::GetTaggedTiles(currTag, TILE_ID::MOVINGTILETARGET);
		if (!targets.empty()) {
			targetPos = targets[0]->pos;
			hasTarget = true;
		}
	}

	void Update() override {
		Tile::Update();
		if (!hasTarget || !isEnabled) {
			rb->velocity.x = 0;
			rb->velocity.y = 0;
			return;
		}
		float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
		// waiting at destination
		if (isWaiting) {
			rb->velocity.x = 0;
			rb->velocity.y = 0;
			if (waitTimer.Update(dt)) {
				isWaiting = false;
				movingToTarget = !movingToTarget;  // flip direction after wait
			}
			return;
		}

		AEVec2 destination = movingToTarget ? targetPos : originPos;
		float dx = destination.x - pos.x;
		float dy = destination.y - pos.y;
		float dist = sqrtf(dx * dx + dy * dy);

		if (dist < 2.0f) {
			// snap and start waiting
			pos.x = destination.x;
			pos.y = destination.y;
			rb->velocity.x = 0;
			rb->velocity.y = 0;
			isWaiting = true;
			waitTimer.Start(waitDuration);
		}
		else {
			rb->velocity.x = (dx / dist) * speed;
			rb->velocity.y = (dy / dist) * speed;
		}
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
	std::vector<MovingTile*> linkedMovingTiles;
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
			currSprite->textureFileName = "Assets/Environment/buttonBlueUnpressed.png";
			break;
		case TILE_ID::BUTTONBLUEPRESSED:
		case TILE_ID::BUTTONBLUETIMEDPRESSED:
			currSprite->textureFileName = "Assets/Environment/buttonBluePressed.png";
			break;
		default:
			currSprite->textureFileName = "Assets/Environment/buttonBlueUnpressed.png";
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
		AudioManager::GetInstance().PlaySFX("buttonSwitch");
		SetTexture();
	}

	void ActivateGates(bool _activate) {
		AudioManager::GetInstance().PlaySFX("gateTrigger");
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

			// get currTag gates that haven't reached target state
			std::vector<Tile*> allGates = MapManager::GetTaggedTiles(currTag, TILE_ID::GATE);
			for (Tile* gate : allGates)
			{
				if (gate->isActive != _activate)
					gateQueue.push_back(gate);
			}

			// get altTag gates that haven't reached inverted target state
			std::vector<Tile*> altGates = MapManager::GetTaggedTiles(altTag, TILE_ID::GATE);
			for (Tile* gate : altGates)
			{
				if (gate->isActive != !_activate)
					gateQueue.push_back(gate);
			}

			if (_activate) {
				std::sort(gateQueue.begin(), gateQueue.end(),
					[](Tile* a, Tile* b) { return a->row > b->row; });
			}
			else {
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
				// check if this gate belongs to altTag
				std::vector<Tile*> altGates = MapManager::GetTaggedTiles(altTag, TILE_ID::GATE);
				bool isAltGate = std::find(altGates.begin(), altGates.end(), gate) != altGates.end();

				bool gateActivate = isAltGate ? !activate : activate;
				gate->isActive = gateActivate;
				gate->isCurrActive = gateActivate;
				if (gate->collider) gate->collider->canCollide = gateActivate;
			}

			if (!gateQueue.empty())
				gateTimer.Start(gateInterval);
			else
				queueRunning = false;
		}
	}

	void Init() override {
		Tile::Init();


		collider->isTrigger = true;
		collider->center.y = 0.35f;
		collider->size.y = 1.25f;

		std::vector<Tile*> gates = MapManager::GetTaggedTiles(currTag, TILE_ID::GATE);
		gatesOriginalState = !gates.empty() && gates[0]->isActive;
		gatesAreActive = gatesOriginalState;


		for (Tile* gate : gates) gate->isActive = gate->isCurrActive;
		for (Tile* gate : MapManager::GetTaggedTiles(altTag, TILE_ID::GATE)) gate->isActive = gate->isCurrActive;

		for (Tile* t : MapManager::GetAltTaggedTiles(currTag, TILE_ID::MOVINGTILEBUTTONMID)) {
			MovingTile* mt = dynamic_cast<MovingTile*>(t);
			if (mt && mt->requiresButton) linkedMovingTiles.push_back(mt);
		}
		for (Tile* t : MapManager::GetAltTaggedTiles(currTag, TILE_ID::MOVINGTILEBUTTONLEFT)) {
			MovingTile* mt = dynamic_cast<MovingTile*>(t);
			if (mt && mt->requiresButton) linkedMovingTiles.push_back(mt);
		}
		for (Tile* t : MapManager::GetAltTaggedTiles(currTag, TILE_ID::MOVINGTILEBUTTONRIGHT)) {
			MovingTile* mt = dynamic_cast<MovingTile*>(t);
			if (mt && mt->requiresButton) linkedMovingTiles.push_back(mt);
		}

		const float TOP_SURFACE_TOLERANCE = 40.0f;

		collider->OnTriggerEnter = [this, TOP_SURFACE_TOLERANCE](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);

			CrateTile* crate = dynamic_cast<CrateTile*>(other->owner);
			bool wasPressed = isPressed;

			if (player) playerCount++;
			if (crate)  crateCount++;

			isPressed = (playerCount + crateCount) > 0;

			if (!wasPressed && isPressed) {
				ToggleButton();
				bool targetState = !gatesOriginalState;
				std::vector<Tile*> gates = MapManager::GetTaggedTiles(currTag, TILE_ID::GATE);
				std::vector<Tile*> altGates = MapManager::GetTaggedTiles(altTag, TILE_ID::GATE);
				if (!gates.empty() || !altGates.empty()) {
					if (queueRunning) {
						hasPendingActivation = true;
						pendingActivate = targetState;
					}
					else {
						gatesAreActive = targetState;
						ActivateGates(targetState);
					}
				}

				for (MovingTile* mt : linkedMovingTiles)
					mt->isEnabled = true;
			}

			};

		collider->OnTriggerExit = [this](Collider* other, int sides) {
			Player* player = dynamic_cast<Player*>(other->owner);
			CrateTile* crate = dynamic_cast<CrateTile*>(other->owner);
			bool wasPressed = isPressed;

			if (player) playerCount = std::max<int>(0, playerCount - 1);
			if (crate)  crateCount = std::max<int>(0, crateCount - 1);

			isPressed = (playerCount + crateCount) > 0;

			if (wasPressed && !isPressed) {
				ToggleButton();

				// Add this check
				std::vector<Tile*> gates = MapManager::GetTaggedTiles(currTag, TILE_ID::GATE);
				std::vector<Tile*> altGates = MapManager::GetTaggedTiles(altTag, TILE_ID::GATE);
				if (!gates.empty() || !altGates.empty()) {
					if (queueRunning) {
						hasPendingActivation = true;
						pendingActivate = gatesOriginalState;
					}
					else {
						gatesAreActive = gatesOriginalState;
						ActivateGates(gatesOriginalState);
					}
				}

				for (MovingTile* mt : linkedMovingTiles)
					mt->isEnabled = false;
			}
			};
	}
};
struct MovingTileTarget : Tile {
	MovingTileTarget(TILE_ID currID_, TILE_ID bgID_, int currTag_,
		bool bgActive, bool currActive, size_t row_, size_t col_, float tileSize)
		: Tile(currID_, bgID_, currTag_, bgActive, currActive, row_, col_, tileSize) {
		isActive = false;
	}
	void Init() override {
		Tile::Init();
	}
};
#endif // !MAP_MANAGER