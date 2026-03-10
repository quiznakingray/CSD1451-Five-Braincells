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

    static void SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& gameObjects);

    static void UpdateAllEnemies(f32 dt);

    static AEVec2 GetPlayerPos();

    // Getter for rendering
    static const std::vector<EnemyGameObject*>& GetEnemies() { return enemies; }
};

#endif