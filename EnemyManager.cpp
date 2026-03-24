#include "EnemyManager.h"
#include <iostream>
#include <cstdlib> // For rand

std::vector<EnemyGameObject*> EnemyManager::enemies;
Player* EnemyManager::player = nullptr;
extern MapManager mapManager;

void EnemyManager::Init(Player* p) {
    player = p;
    enemies.clear();
    std::cout << "[EnemyManager] Initialized\n";
}

void EnemyManager::RegisterEnemy(EnemyGameObject* enemy) {
    enemies.push_back(enemy);
    std::cout << "[EnemyManager] Enemy Registered. Total: " << enemies.size() << "\n";
}

AEVec2 EnemyManager::GetPlayerPos() {
    return player ? player->pos : AEVec2{ 0, 0 };
}

void EnemyManager::SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& gameObjects) {
    std::vector<Tile*> enemyTiles = mapManager.GetTilesWithID(TILE_ID::ENEMY);

    int spawned = 0;
    for (Tile* tile : enemyTiles) {
        if (spawned >= numBasic + numMiniBoss) break;

        EnemyType type = (spawned < numBasic) ? EnemyType::BASIC : EnemyType::MINI_BOSS;
        EnemyGameObject* enemy = new EnemyGameObject();
        //enemy->Init(type, tile);
        enemy->base.type = type;
        AddGameObjectToVector(enemy, gameObjects);
        RegisterEnemy(enemy);
        spawned++;
    }

    std::cout << "[EnemyManager] Spawned " << spawned << " enemies\n";
}

void EnemyManager::UpdateAllEnemies(f32 dt) {
    for (EnemyGameObject* enemy : enemies) {
        if (!enemy->base.isAlive) continue;

        AEVec2 playerPos = GetPlayerPos();

        // Check line of sight (simple distance check)
        f32 dx = playerPos.x - enemy->pos.x;
        f32 dy = playerPos.y - enemy->pos.y;
        f32 distance = sqrtf(dx * dx + dy * dy);

        if (distance < 500.f) { // Player detected
            enemy->FollowPlayer(playerPos, dt);
        }
        else { // Patrol
            enemy->Patrol(dt);
        }
    }
}

void EnemyManager::RenderEnemies()
{
    for (EnemyGameObject* enemy : enemies) {
        enemy->Render();
    }
}
