#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "AEEngine.h"
#include "EnemyGameObject.h"
#include "MapManager.h"
#include <vector>

struct EnemyManager {
private:
    static std::vector<EnemyGameObject*> enemies;
    static Player* player;

public:
    static void Init(Player* p);

    static void RegisterEnemy(EnemyGameObject* enemy);

    static void SpawnEnemies(int numBasic, int numMiniBoss);

    static void UpdateAllEnemies(f32 dt);

    static AEVec2 GetPlayerPos();
};

#endif