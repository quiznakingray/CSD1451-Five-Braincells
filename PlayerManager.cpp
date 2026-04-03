#include "PlayerManager.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "CameraSystem.h"
#include "PlayerStats.h"
#include <fstream>
#include <direct.h>

//Arrow* PlayerManager::rangePlayerArrow = nullptr;


void PlayerManager::Init()
{
	meleePlayer = new MeleePlayer;
	AEVec2Set(&meleePlayer->pos, MapManager::GetPlayerSpawnPos().x, MapManager::GetPlayerSpawnPos().y + 200.f);
	rangedPlayer = new RangePlayer;
	AEVec2Set(&rangedPlayer->pos, MapManager::GetPlayerSpawnPos().x + MapManager::tileSize, MapManager::GetPlayerSpawnPos().y + 200.f);
	//rangePlayerArrow = new Arrow;
	//AEVec2Set(&rangePlayerArrow->pos, MapManager::GetPlayerSpawnPos().x + MapManager::tileSize, MapManager::GetPlayerSpawnPos().y + 200.f);
	for (size_t i = 0; i < maxArrowPoolSize; i++)
	{
		Arrow* a = new Arrow;
		AEVec2Set(&a->pos, MapManager::GetPlayerSpawnPos().x + MapManager::tileSize, MapManager::GetPlayerSpawnPos().y + 200.f);
		arrowGameObjectPool.push_back(a);
	}
	currentPlayer = PlayerStats::GetInstance().GetPlayerType() == PLAYER_TYPE::MELEE
		? static_cast<Player*>(meleePlayer)
		: static_cast<Player*>(rangedPlayer);
	CameraSystem::SetCameraPos(meleePlayer->pos);

	if (!SaveManager::GetInstance().HasSaveData()) {
		PlayerStats::GetInstance().ResetHealthStamina();
	}
}

void PlayerManager::Update(){
	if (!currentPlayer) return;
	double dt = AEFrameRateControllerGetFrameTime();
	currentPlayer->PlayerInput();
	//currentPlayer->PlayerAction();
	
	if (currentPlayer != meleePlayer) {
		meleePlayer->ApplyDeceleration();
		meleePlayer->shieldActive = false;
		meleePlayer->inShieldAction = false;
	}
	if (currentPlayer != rangedPlayer) {
		rangedPlayer->ApplyDeceleration();
		rangedPlayer->aiming = false;
	}

	if (AEInputCheckCurr(AEVK_E) && currentPlayer != meleePlayer)
	{
		ChangePlayer(PLAYER_TYPE::MELEE);
	}
	else if (AEInputCheckCurr(AEVK_R) && currentPlayer != rangedPlayer)
	{
		ChangePlayer(PLAYER_TYPE::RANGE);
	}
	rangedPlayer->line->isActive = currentPlayer == rangedPlayer && currentPlayer->currentAction == PLAYER_ACTION::AIMING;

	//regen stamina

	PlayerStats::GetInstance().RegenStamina(dt);
	

	if (!canChangePlayer)
	{
		StartPlayerCooldown(dt);
	}
	else {
		playerSwitchingCooldown = playerSwitchingDuration;
	}
	CameraSystem::LerpToPosition(currentPlayer->pos);

}
void PlayerManager::Render(){
	if (!currentPlayer) return;
	meleePlayer->Render();
	rangedPlayer->Render();
	//rangePlayerArrow->Render();
	for (Arrow* a : arrowGameObjectPool)
	{
		a->Render();
	}
}

void PlayerManager::Free() {
	if (meleePlayer) {
		meleePlayer->Free();
		delete meleePlayer;
		meleePlayer = nullptr;
	}
	if (rangedPlayer) {
		rangedPlayer->Free();
		delete rangedPlayer;
		rangedPlayer = nullptr;
	}
	//if (rangePlayerArrow) {
	//	rangePlayerArrow->Free();
	//	delete rangePlayerArrow;
	//	rangePlayerArrow = nullptr;
	//}
	for (Arrow* a : arrowGameObjectPool)
	{
		if (!a) continue;
		a->Free();
		delete a;
	}
	currentPlayer = nullptr;
}

void PlayerManager::SavePlayerData()
{
	SaveManager& save = SaveManager::GetInstance();

	// positions
	if (meleePlayer) save.playerSaveData.meleePos = meleePlayer->pos;
	if (rangedPlayer) save.playerSaveData.rangedPos = rangedPlayer->pos;

	// copy persistent stats from PlayerStats singleton
	PlayerStats& stats = PlayerStats::GetInstance();
	save.playerSaveData.health = stats.health;
	save.playerSaveData.maxHealth = stats.maxHealth;
	save.playerSaveData.damage = stats.damage;
	save.playerSaveData.proficiency = stats.proficiency;
	save.playerSaveData.speedMult = stats.speedMult;

	save.playerSaveData.maxJumpStamina = stats.maxJumpStamina;
	save.playerSaveData.jumpStamina = stats.jumpStamina;

	save.playerSaveData.deathCount = stats.deathCount;
	save.playerSaveData.killCount = stats.killCount;
	save.playerSaveData.totalSeconds = stats.totalSeconds;

	save.playerSaveData.currentPlayerType = stats.playerType;

	save.playerSaveData.hasSavedData = true;
}

void PlayerManager::Load()
{
	PlayerSaveData& data = SaveManager::GetInstance().playerSaveData;
	if (!data.hasSavedData) return;

	// restore positions to game objects
	if (meleePlayer) {
		AEVec2Set(&meleePlayer->pos, data.meleePos.x, data.meleePos.y);
	}
	if (rangedPlayer) {
		AEVec2Set(&rangedPlayer->pos, data.rangedPos.x, data.rangedPos.y);
	}

	// restore stats into PlayerStats singleton
	PlayerStats& stats = PlayerStats::GetInstance();
	
	stats.health = data.health;
	stats.maxHealth = data.maxHealth;
	stats.damage = data.damage;
	stats.proficiency = data.proficiency;
	stats.speedMult = data.speedMult;

	stats.maxJumpStamina = data.maxJumpStamina;
	stats.jumpStamina = data.jumpStamina;

	stats.deathCount = data.deathCount;
	stats.killCount = data.killCount;
	stats.totalSeconds = data.totalSeconds;

	stats.playerType = data.currentPlayerType;
	currentPlayer = PlayerStats::GetInstance().GetPlayerType() == PLAYER_TYPE::MELEE
		? static_cast<Player*>(meleePlayer)
		: static_cast<Player*>(rangedPlayer);
}


void PlayerManager::ChangePlayer(PLAYER_TYPE type)
{
	if (!canChangePlayer) return;
	currentPlayerType = type;
	currentPlayer = type == PLAYER_TYPE::MELEE
		? static_cast<Player*>(meleePlayer)
		: static_cast<Player*>(rangedPlayer);
	canChangePlayer = false;
	PlayerStats::GetInstance().SetPlayerType(currentPlayerType);
	EnemyManager::GetInstance().SetTarget(currentPlayer);
}



void PlayerManager::StartPlayerCooldown(f64 dt)
{
	playerSwitchingCooldown -= static_cast<f32>(dt);
	if (playerSwitchingCooldown <= 0)
	{
		canChangePlayer = true;
	}
}
