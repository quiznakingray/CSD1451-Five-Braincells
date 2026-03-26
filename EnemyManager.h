#ifndef ENEMY_MANAGER_H
#define ENEMY_MANAGER_H

#include "AEEngine.h"
#include "EnemyGameObject.h"
#include "MapManager.h"
#include "SingletonTemplate.h"
#include <vector>

struct EnemyManager : public Singleton<EnemyManager> {
private:
    friend class Singleton<EnemyManager>;

    std::vector<EnemyGameObject*> enemies;
    Player* player;

public:
    void Init(Player* p);

    void RegisterEnemy(EnemyGameObject* enemy);

    void SpawnEnemies(int numBasic, int numMiniBoss, std::vector<GameObject*>& gameObjects);

    void UpdateAllEnemies(f32 dt);

    void RenderEnemies();

    AEVec2 GetPlayerPos();

    static void SaveEnemyStates();

    static void LoadEnemyStates();

    // Getter for rendering
    const std::vector<EnemyGameObject*>& GetEnemies() { return enemies; }
};

#endif