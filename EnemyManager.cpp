#include "EnemyManager.h"
#include "SaveManager.h"
#include <iostream>
#include <algorithm>

extern MapManager mapManager;

void EnemyManager::Init(MeleePlayer* p1, RangePlayer* p2) {
    player1 = p1;
    player2 = p2;
    //FreeEnemies();
    enemies.clear();
    registeredEnemies.clear();
    std::cout << "[EnemyManager] Initialized\n";
}

void EnemyManager::FreeEnemies() {
    //for (EnemyGameObject* enemy : enemies) {
    //    delete enemy;
    //}
    enemies.clear();
    registeredEnemies.clear();
}

void EnemyManager::RegisterEnemy(EnemyGameObject* enemy) {
    if (!enemy || registeredEnemies.count(enemy)) return;
    enemies.push_back(enemy);
    registeredEnemies.insert(enemy);
}

AEVec2 EnemyManager::GetClosestPlayerPos(const AEVec2& enemyPos) {
    if (!player1 && !player2) return { 0,0 };
    if (!player1) return player2->pos;
    if (!player2) return player1->pos;

    float dx1 = player1->pos.x - enemyPos.x;
    float dy1 = player1->pos.y - enemyPos.y;
    float dist1 = dx1 * dx1 + dy1 * dy1;

    float dx2 = player2->pos.x - enemyPos.x;
    float dy2 = player2->pos.y - enemyPos.y;
    float dist2 = dx2 * dx2 + dy2 * dy2;

    return (dist1 < dist2) ? player1->pos : player2->pos;
}

void EnemyManager::SaveEnemyStates() {
    auto& save = SaveManager::GetInstance();
    save.enemySaveData.clear();

    for (EnemyGameObject* enemy : enemies) {
        if (!enemy) continue;

        EnemySaveData data;
        data.pos = enemy->pos;

        // Save full enemy stats and state
        data.type = enemy->base.type;
        data.stats = enemy->base.stats;
        data.currentState = enemy->base.currentState;
        data.isAlive = enemy->base.isAlive;
        data.canMove = enemy->base.canMove;
        data.patrolStart = enemy->base.patrolStart;
        data.patrolEnd = enemy->base.patrolEnd;
        data.currentTarget = enemy->base.currentTarget;
        data.timeSinceLastAttack = enemy->base.timeSinceLastAttack;

        save.enemySaveData.push_back(data);
    }

    save.SaveEnemyData();
}

void EnemyManager::LoadEnemyStates() {
    auto& save = SaveManager::GetInstance();
    auto& savedData = save.enemySaveData;

    if (savedData.empty()) return;

    for (size_t i = 0; i < enemies.size() && i < savedData.size(); i++) {
        EnemyGameObject* enemy = enemies[i];
        const EnemySaveData& data = savedData[i];

        if (!enemy) continue;

        enemy->pos = data.pos;

        // Restore full enemy stats and state
        enemy->base.type = data.type;
        enemy->base.stats = data.stats;
        enemy->base.currentState = data.currentState;
        enemy->base.isAlive = data.isAlive;
        enemy->base.canMove = data.canMove;
        enemy->base.patrolStart = data.patrolStart;
        enemy->base.patrolEnd = data.patrolEnd;
        enemy->base.currentTarget = data.currentTarget;
        enemy->base.timeSinceLastAttack = data.timeSinceLastAttack;

        enemy->isActive = enemy->base.isAlive;
    }
}

void EnemyManager::SpawnEnemy(EnemyType type, Tile* tile, std::vector<GameObject*>& gameObjects) {
    if (!tile) return;
    EnemyGameObject* enemy = new EnemyGameObject();
    enemy->Init(type, tile);
    gameObjects.push_back(enemy);
    RegisterEnemy(enemy);
}

void EnemyManager::SpawnEnemies(std::vector<GameObject*>& goVec) {
    std::unordered_set<Tile*> occupied;

    // Get tiles for each enemy type
    auto basicMeleeTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::ENEMYMELEE);
    auto basicRangedTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::ENEMYRANGE);
    auto miniBossMeleeTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::MINIBOSSMELEE);
    auto miniBossRangedTiles = MapManager::GetInstance().GetTilesWithID(TILE_ID::MINIBOSSRANGE);

    // Spawn BASIC MELEE
    for (Tile* tile : basicMeleeTiles)
    {
        if (!tile || occupied.count(tile)) continue;
        SpawnEnemy(EnemyType::BASIC_MELEE, tile, goVec);
        occupied.insert(tile);
    }

    // Spawn BASIC RANGED
    for (Tile* tile : basicRangedTiles)
    {
        if (!tile || occupied.count(tile)) continue;
        SpawnEnemy(EnemyType::BASIC_RANGED, tile, goVec);
        occupied.insert(tile);
    }

    // Spawn MINI BOSS MELEE
    for (Tile* tile : miniBossMeleeTiles)
    {
        if (!tile || occupied.count(tile)) continue;
        SpawnEnemy(EnemyType::MINI_BOSS_MELEE, tile, goVec);
        occupied.insert(tile);
    }

    // Spawn MINI BOSS RANGED
    for (Tile* tile : miniBossRangedTiles)
    {
        if (!tile || occupied.count(tile)) continue;
        SpawnEnemy(EnemyType::MINI_BOSS_RANGED, tile, goVec);
        occupied.insert(tile);
    }
}

void EnemyManager::RenderEnemies() {
    for (EnemyGameObject* enemy : enemies) {
        if (enemy) enemy->Render();
    }
}