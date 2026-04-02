#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "AEEngine.h"
#include "SingletonTemplate.h"
#include <direct.h>
#include <string>
#include <vector>

// Forward declarations to avoid circular include with MapManager.h
enum class TILE_ID;          // MapManager defines TILE_ID
#include "GameStateManager.h"
#include "EnemyBase.h"

struct PlayerSaveData {
    AEVec2 meleePos{};
    AEVec2 rangedPos{};
    // persisted stats
    int health = 3;
    int maxHealth = 3;
    int damage = 1;
    float proficiency = 0.0f;
    float speedMult = 1.0f;

    // stamina fields
    int maxJumpStamina = 3;
    int jumpStamina = 3;

    // counters
    int deathCount = 0;
    int killCount = 0;

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

struct EnemySaveData {
    AEVec3 pos{};
    EnemyType type{}; // EnemyType as int
    EnemyStats stats{};
    EnemyState currentState = EnemyState::IDLE;
    bool isAlive = true;
    bool canMove = true;
    AEVec2 patrolStart{};
    AEVec2 patrolEnd{};
    AEVec2 currentTarget{};
    float timeSinceLastAttack = 0.f;
};

struct AudioStateData {
    float masterVolume = 1.0f;
    float musicVolume = 1.0f;
    float sfxVolume = 1.0f;
    bool preserveOnLoad = false;
    bool hasSavedData = false;
};

struct SaveManager : public Singleton<SaveManager>
{
    PlayerSaveData playerSaveData;
    MapSaveData    mapSaveData;
    std::vector<EnemySaveData> enemySaveData;
    AudioStateData audioSaveData;
    bool toContinue = false;

    void SavePlayerData();
    void LoadPlayerData();
    void SetPreservePlayerOnLoad(bool preserve);
    void SaveMapData();
    void LoadMapData();
    void SaveEnemyData();
    void LoadEnemyData();
    void SaveAudioData();
    void LoadAudioData();
    void SaveAll();
    void LoadAll();
    bool HasSaveData();
    void ResetSave();
};

#endif // SAVE_MANAGER_H
