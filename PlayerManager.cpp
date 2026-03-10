#include "PlayerManager.h"
#include "MapManager.h"

void PlayerManager::Init()
{
	meleePlayer = new Player;
	AEVec2Set(&meleePlayer->pos, MapManager::GetPlayerSpawnPos().x, MapManager::GetPlayerSpawnPos().y + 200.f);
	rangedPlayer = new Player;
	AEVec2Set(&rangedPlayer->pos, MapManager::GetPlayerSpawnPos().x + MapManager::tileSize, MapManager::GetPlayerSpawnPos().y + 200.f);

	currentPlayer = meleePlayer;
}

void PlayerManager::Update(){
	if (!currentPlayer) return;
	currentPlayer->PlayerInput();
}
void PlayerManager::Render(){

}
void PlayerManager::Free(){
	if (meleePlayer) delete meleePlayer;
	if (rangedPlayer) delete rangedPlayer;
}