#include "PlayerManager.h"
#include "MapManager.h"

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
	camPos = meleePlayer->pos;
}

void PlayerManager::Update(){
	if (!currentPlayer) return;
	currentPlayer->PlayerInput();
	
	if (AEInputCheckCurr(AEVK_COMMA) && currentPlayer != meleePlayer)
	{
		ChangePlayer(PLAYER_TYPE::MELEE);
	}
	else if (AEInputCheckCurr(AEVK_PERIOD) && currentPlayer != rangedPlayer)
	{
		ChangePlayer(PLAYER_TYPE::RANGE);

	}

	rangedPlayer->line->isActive = currentPlayer == rangedPlayer && currentPlayer->currentAction == PlayerAction::AIMING;

	AEVec2 target = currentPlayer->pos;

	// Lerp camera
	camPos.x += (target.x - camPos.x) * camLerpSpeed * AEFrameRateControllerGetFrameTime();
	camPos.y += (target.y - camPos.y) * camLerpSpeed * AEFrameRateControllerGetFrameTime();

	AEGfxSetCamPosition(camPos.x, camPos.y);

}
void PlayerManager::Render(){
	meleePlayer->Render();
	rangedPlayer->Render();
	rangePlayerArrow->Render();
}

void PlayerManager::ChangePlayer(PLAYER_TYPE type)
{
	currentPlayer = type == PLAYER_TYPE::MELEE
		? static_cast<Player*>(meleePlayer)
		: static_cast<Player*>(rangedPlayer);
}
