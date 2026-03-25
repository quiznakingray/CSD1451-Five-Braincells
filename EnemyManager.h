#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "AEEngine.h"
#include "EnemyGameObject.h"
#include "MapManager.h"
#include "SingletonTemplate.h"
#include <vector>

struct EnemyManager : public Singleton<EnemyManager> {
private:
    static std::vector<EnemyGameObject*> enemies;
    static Player* player;

public:
    static void Init(Player* p);

    static void RegisterEnemy(EnemyGameObject* enemy);

    static void SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& gameObjects);

    static void UpdateAllEnemies(f32 dt);

    static void RenderEnemies();

    static AEVec2 GetPlayerPos();

    static void SaveEnemyStates();

    static void LoadEnemyStates();

    // Getter for rendering
    static const std::vector<EnemyGameObject*>& GetEnemies() { return enemies; }
};

#endif