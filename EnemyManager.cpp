#include "EnemyManager.h"
#include "SaveManager.h"
#include <iostream>
#include <cstdlib> // For rand
#include <unordered_set>
#include <algorithm>

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

void EnemyManager::SpawnEnemy(EnemyType type, Tile* tile, std::vector<GameObject*>& gameObjects)
{
    EnemyGameObject* enemy = new EnemyGameObject();
    enemy->Init(type, tile);

    AddGameObjectToVector(enemy, gameObjects);
    RegisterEnemy(enemy);
}

void EnemyManager::SpawnEnemies(int numEnemies, int numMiniBoss, std::vector<GameObject*>& goVec)
{
    auto enemyTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::ENEMY);
    auto miniBossTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::ENEMY); // or another tileID for bosses

    std::unordered_set<Tile*> occupied;

    // clamp numbers to available tiles
    numEnemies = min(numEnemies, (int)enemyTiles.size());
    numMiniBoss = min(numMiniBoss, (int)miniBossTiles.size());

    // spawn normal enemies
    for (int i = 0; i < numEnemies; i++)
    {
        Tile* tile = enemyTiles[i];
        if (occupied.find(tile) != occupied.end()) continue;

        EnemyGameObject* enemy = new EnemyGameObject();
        enemy->Init(EnemyType::BASIC_MELEE, tile);
        goVec.push_back(enemy);
        occupied.insert(tile);
        RegisterEnemy(enemy);
    }

    // spawn mini bosses
    for (int i = 0; i < numMiniBoss; i++)
    {
        Tile* tile = miniBossTiles[i];
        if (occupied.find(tile) != occupied.end()) continue;

        EnemyGameObject* boss = new EnemyGameObject();
        boss->Init(EnemyType::MINI_BOSS_MELEE, tile); // or MINI_BOSS_RANGED
        goVec.push_back(boss);
        occupied.insert(tile);
        RegisterEnemy(boss);
    }

    std::cout << "[EnemyManager] Spawn complete\n";
}

void EnemyManager::UpdateAllEnemies(f64 dt) {
    for (EnemyGameObject* enemy : enemies) {
        if (!enemy->base.isAlive) continue;

        AEVec2 playerPos = GetPlayerPos();

        // Check line of sight
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
