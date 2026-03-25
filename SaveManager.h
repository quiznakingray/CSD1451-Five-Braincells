#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "AEEngine.h"
#include "SingletonTemplate.h"
#include <direct.h>
#include <string>
#include <filesystem>
#include "MapManager.h"
#include "GameStateManager.h"

struct PlayerSaveData {
	AEVec2 meleePos{};
	AEVec2 rangedPos{};
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

struct SaveManager : public Singleton<SaveManager>
{
    PlayerSaveData playerSaveData;
    MapSaveData mapSaveData;
    bool toContinue = false;

    void SavePlayerData(AEVec2 meleePos, AEVec2 rangedPos);
    void LoadPlayerData();
    void SetPreservePlayerOnLoad(bool preserve);
    void SaveMapData();
    void LoadMapData();
    bool HasSaveData();
    void ResetSave();
};
#endif

