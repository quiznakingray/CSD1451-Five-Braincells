#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H
#include "AEEngine.h"
#include "SingletonTemplate.h"
#include <direct.h>
#include <string>
#include "MapManager.h"
#include "GameStateManager.h"
#include "EnemyBase.h"

struct PlayerSaveData {
    AEVec2 meleePos{};
    AEVec2 rangedPos{};
    int health = 5;
    bool preserveOnLoad = false;
    bool hasSavedData = false;
};

struct TileStateData {
    size_t row{};
    size_t col{};
    TILE_ID currID{};
    bool isActive{};
    bool isCurrActive{};
    bool colliderCanCollide{};
    AEVec3 pos{};
};

struct MapSaveData {
    std::vector<TileStateData> tileStates;
    GAME_STATE_TYPE savedLevel = GAME_STATE_TYPE::LEVEL1;
    bool hasSavedData = false;
};

// one entry per enemy, trivially copyable for binary serialization
struct EnemySaveData {
    AEVec3 pos{};
    EnemyBase enemyBase{};
};

struct SaveManager : public Singleton<SaveManager>
{
    PlayerSaveData playerSaveData;
    MapSaveData    mapSaveData;
    std::vector<EnemySaveData> enemySaveData;  // vector lives here
    bool toContinue = false;

    void SavePlayerData();
    void LoadPlayerData();
    void SetPreservePlayerOnLoad(bool preserve);
    void SaveMapData();
    void LoadMapData();
    void SaveEnemyData();
    void LoadEnemyData();
    void SaveAll();
    void LoadAll();
    bool HasSaveData();
    void ResetSave();
};

#endif