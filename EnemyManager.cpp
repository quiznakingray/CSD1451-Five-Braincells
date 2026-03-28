#include "EnemyManager.h"
#include "SaveManager.h"
#include <iostream>
#include <cstdlib> // For rand

extern MapManager mapManager;

EnemyManager::~EnemyManager()
{
    enemies.clear();
}

void EnemyManager::Init(Player* p) {
    player = p;
    enemies.clear();
    std::cout << "[EnemyManager] Initialized\n";
}

void EnemyManager::RegisterEnemy(EnemyGameObject* enemy) {
    if (!enemy) return;
    if (std::find(enemies.begin(), enemies.end(), enemy) != enemies.end()) return;
    enemies.push_back(enemy);
    std::cout << "[EnemyManager] Enemy Registered. Total: " << enemies.size() << "\n";
}

AEVec2 EnemyManager::GetPlayerPos() {
    return player ? player->pos : AEVec2{ 0, 0 };
}

void EnemyManager::SaveEnemyStates()
{
    SaveManager::GetInstance().enemySaveData.clear();
    for (EnemyGameObject* enemy : enemies)
    {
        EnemySaveData data;
        data.pos = enemy->pos;
        data.enemyBase = enemy->base;
        SaveManager::GetInstance().enemySaveData.push_back(data);
    }
    SaveManager::GetInstance().SaveEnemyData();
}

void EnemyManager::LoadEnemyStates()
{
    std::vector<EnemySaveData>& savedData = SaveManager::GetInstance().enemySaveData;
    if (savedData.empty()) return;

    for (size_t i = 0; i < enemies.size() && i < savedData.size(); i++)
    {
        enemies[i]->base = savedData[i].enemyBase;
        enemies[i]->pos = savedData[i].pos;

        if (!savedData[i].enemyBase.isAlive)
        {
            enemies[i]->isActive = false;
        }
    }
}

void EnemyManager::SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& gameObjects) {
    std::vector<Tile*> enemyTiles = mapManager.GetTilesWithID(TILE_ID::ENEMY);

    int spawned = 0;
    for (Tile* tile : enemyTiles) {
        if (spawned >= numBasic + numMiniBoss) break;

        EnemyType type = (spawned < numBasic) ? EnemyType::BASIC : EnemyType::MINI_BOSS;
        EnemyGameObject* enemy = new EnemyGameObject();
        enemy->Init(type, tile);
        AddGameObjectToVector(enemy, gameObjects);
        RegisterEnemy(enemy);
        spawned++;
    }

    std::cout << "[EnemyManager] Spawned " << spawned << " enemies\n";
}

void EnemyManager::UpdateAllEnemies(f64 dt) {
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
