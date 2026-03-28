#include "PlayerManager.h"
#include "MapManager.h"
#include "SaveManager.h"
#include "CameraSystem.h"
#include "PlayerStats.h"
#include <fstream>
#include <direct.h>

Arrow* PlayerManager::rangePlayerArrow = nullptr;


void PlayerManager::Init()
{
	meleePlayer = new MeleePlayer;
	AEVec2Set(&meleePlayer->pos, MapManager::GetPlayerSpawnPos().x, MapManager::GetPlayerSpawnPos().y + 200.f);
	rangedPlayer = new RangePlayer;
	AEVec2Set(&rangedPlayer->pos, MapManager::GetPlayerSpawnPos().x + MapManager::tileSize, MapManager::GetPlayerSpawnPos().y + 200.f);
	rangePlayerArrow = new Arrow;
	AEVec2Set(&rangePlayerArrow->pos, MapManager::GetPlayerSpawnPos().x + MapManager::tileSize, MapManager::GetPlayerSpawnPos().y + 200.f);

	currentPlayer = meleePlayer;
	CameraSystem::SetCameraPos(meleePlayer->pos);
}

void PlayerManager::Update(){
	if (!currentPlayer) return;
	double dt = AEFrameRateControllerGetFrameTime();
	currentPlayer->PlayerInput();
	currentPlayer->PlayerAction();
	
	if (currentPlayer != meleePlayer) meleePlayer->ApplyDeceleration();
	if (currentPlayer != rangedPlayer) rangedPlayer->ApplyDeceleration();

	if (AEInputCheckCurr(AEVK_COMMA) && currentPlayer != meleePlayer)
	{
		ChangePlayer(PLAYER_TYPE::MELEE);
	}
	else if (AEInputCheckCurr(AEVK_PERIOD) && currentPlayer != rangedPlayer)
	{
		ChangePlayer(PLAYER_TYPE::RANGE);

	}
	if (AEInputCheckTriggered(AEVK_L) && PlayerStats::Get().health != 0)
	{
		--PlayerStats::Get().health;
	}
	rangedPlayer->line->isActive = currentPlayer == rangedPlayer && currentPlayer->currentAction == PLAYER_ACTION::AIMING;

	//regen stamina

	PlayerStats::Get().RegenStamina(dt); 
	

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
	meleePlayer->Render();
	rangedPlayer->Render();
	rangePlayerArrow->Render();
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
	if (rangePlayerArrow) {
		rangePlayerArrow->Free();
		delete rangePlayerArrow;
		rangePlayerArrow = nullptr;
	}
	currentPlayer = nullptr;
}

void PlayerManager::SavePlayerData()
{
	SaveManager& save = SaveManager::GetInstance();

	save.playerSaveData.meleePos = meleePlayer->pos;
	save.playerSaveData.rangedPos = rangedPlayer->pos;
	save.playerSaveData.health = meleePlayer->health;
	save.playerSaveData.hasSavedData = true;

}
void PlayerManager::Load()
{
	PlayerSaveData& data = SaveManager::GetInstance().playerSaveData;
	if (!data.hasSavedData) return;

	meleePlayer->health = data.health;
	AEVec2Set(&meleePlayer->pos, data.meleePos.x, data.meleePos.y);
	AEVec2Set(&rangedPlayer->pos, data.rangedPos.x, data.rangedPos.y);
}

void PlayerManager::ChangePlayer(PLAYER_TYPE type)
{
	if (!canChangePlayer) return;
	currentPlayerType = type;
	currentPlayer = type == PLAYER_TYPE::MELEE
		? static_cast<Player*>(meleePlayer)
		: static_cast<Player*>(rangedPlayer);
	canChangePlayer = false;
}



void PlayerManager::StartPlayerCooldown(f64 dt)
{
	playerSwitchingCooldown -= static_cast<f32>(dt);
	if (playerSwitchingCooldown <= 0)
	{
		canChangePlayer = true;
	}
}
