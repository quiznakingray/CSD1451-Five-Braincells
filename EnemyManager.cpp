#include "EnemyManager.h"
#include "SaveManager.h"
#include <iostream>
#include <algorithm>

extern MapManager mapManager;

void EnemyManager::Init(Player* p) {
    player = p;
    //FreeEnemies();
    enemies.clear();
    registeredEnemies.clear();
    std::cout << "[EnemyManager] Initialized\n";
}

void EnemyManager::FreeEnemies() {
    for (EnemyGameObject* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();
    registeredEnemies.clear();
}

void EnemyManager::RegisterEnemy(EnemyGameObject* enemy) {
    if (!enemy || registeredEnemies.count(enemy)) return;
    enemies.push_back(enemy);
    registeredEnemies.insert(enemy);
}

AEVec2 EnemyManager::GetPlayerPos() {
    return player ? player->pos : AEVec2{ 0, 0 };
}

void EnemyManager::SaveEnemyStates() {
    SaveManager::GetInstance().enemySaveData.clear();
    for (EnemyGameObject* enemy : enemies) {
        EnemySaveData data;
        data.pos = enemy->pos;
        data.enemyBase = enemy->base;
        SaveManager::GetInstance().enemySaveData.push_back(data);
    }
    SaveManager::GetInstance().SaveEnemyData();
}

void EnemyManager::LoadEnemyStates() {
    auto& savedData = SaveManager::GetInstance().enemySaveData;
    if (savedData.empty()) return;

    for (size_t i = 0; i < enemies.size() && i < savedData.size(); i++) {
        enemies[i]->base = savedData[i].enemyBase;
        enemies[i]->pos = savedData[i].pos;
        enemies[i]->isActive = savedData[i].enemyBase.isAlive;
    }
}

void EnemyManager::SpawnEnemy(EnemyType type, Tile* tile, std::vector<GameObject*>& gameObjects) {
    if (!tile) return;
    EnemyGameObject* enemy = new EnemyGameObject();
    enemy->Init(type, tile);
    gameObjects.push_back(enemy);
    RegisterEnemy(enemy);
}

void EnemyManager::SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& goVec) {
    auto enemyTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::ENEMY);
    auto miniBossTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::ENEMY);
    std::unordered_set<Tile*> occupied;

    numBasic = min(numBasic, (int)enemyTiles.size());
    numMiniBoss = min(numMiniBoss, (int)miniBossTiles.size());

    for (int i = 0; i < numBasic; i++) {
        Tile* tile = enemyTiles[i];
        if (occupied.count(tile)) continue;
        SpawnEnemy(EnemyType::BASIC_MELEE, tile, goVec);
        occupied.insert(tile);
    }

    for (int i = 0; i < numMiniBoss; i++) {
        Tile* tile = miniBossTiles[i];
        if (occupied.count(tile)) continue;
        SpawnEnemy(EnemyType::MINI_BOSS_MELEE, tile, goVec); // could parameterize for ranged
        occupied.insert(tile);
    }

    std::cout << "[EnemyManager] Spawn complete. Total enemies: " << enemies.size() << "\n";
}

void EnemyManager::RenderEnemies() {
    for (EnemyGameObject* enemy : enemies) {
        if (enemy) enemy->Render();
    }
}