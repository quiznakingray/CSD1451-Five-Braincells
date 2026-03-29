#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "AEEngine.h"
#include "EnemyGameObject.h"
#include "MapManager.h"
#include "SingletonTemplate.h"
#include <vector>
#include <unordered_set>

struct EnemyManager : public Singleton<EnemyManager> {
private:
    friend class Singleton<EnemyManager>;
    std::vector<EnemyGameObject*> enemies;
    std::unordered_set<EnemyGameObject*> registeredEnemies;

public:
    Player* player = nullptr;

    void Init(Player* p);
    void FreeEnemies();

    void RegisterEnemy(EnemyGameObject* enemy);

    void SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& gameObjects);
    void SpawnEnemy(EnemyType type, Tile* tile, std::vector<GameObject*>& gameObjects);

    void RenderEnemies();

    AEVec2 GetPlayerPos();

    void SaveEnemyStates();
    void LoadEnemyStates();

    const std::vector<EnemyGameObject*>& GetEnemies() { return enemies; }
};

#endif