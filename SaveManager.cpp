#include "SaveManager.h"
#include "MapManager.h"
#include "EnemyManager.h"
#include "PlayerManager.h"
#include <string>
#include <fstream>
#include <iostream>

void SaveManager::SavePlayerData()
{
    std::cout << "Saving to: Assets/Saves/playerSave.dat\n";

    std::ofstream file("Assets/Saves/playerSave.dat", std::ios::binary);
    if (file.is_open())
    {
        file.write(reinterpret_cast<char*>(&playerSaveData), sizeof(PlayerSaveData));
        file.close();
        std::cout << "Saved successfully\n";
    }
    else
    {
        std::cout << "Failed to save player data\n";
    }
}

void SaveManager::LoadPlayerData()
{
    std::cout << "Loading from: " << "Assets/Saves/playerSave.dat" << '\n';

    std::ifstream file("Assets/Saves/playerSave.dat", std::ios::binary);
    if (file.is_open())
    {
        file.read(reinterpret_cast<char*>(&playerSaveData), sizeof(PlayerSaveData));
        file.close();
        std::cout << "Loaded successfully\n";
    }
    else
    {
        std::cout << "No save file found at: " <<  "Assets/Saves/playerSave.dat" << '\n';
    }
}

void SaveManager::SetPreservePlayerOnLoad(bool preserve)
{
    playerSaveData.preserveOnLoad = preserve;
}

void SaveManager::SaveDeathCount(int count)
{
    playerSaveData.deathCount = count;
    SavePlayerData();
}

void SaveManager::SaveKillCount(int count) {
    playerSaveData.killCount = count;
    SavePlayerData();
}

void SaveManager::SavePlayerTime(float seconds) {
    playerSaveData.totalSeconds = seconds;
    SavePlayerData();
}

void SaveManager::SaveHighScore(int score) {
    playerSaveData.highScore = score;
    SavePlayerData();
}

void SaveManager::SaveMapData()
{
    std::ofstream file("Assets/Saves/mapSave.dat", std::ios::binary);
    if (file.is_open())
    {
        // Save which level this data belongs to
        file.write(reinterpret_cast<char*>(&mapSaveData.savedLevel), sizeof(GAME_STATE_TYPE));

        // Save tile states
        size_t count = mapSaveData.tileStates.size();
        file.write(reinterpret_cast<char*>(&count), sizeof(size_t));
        file.write(reinterpret_cast<char*>(mapSaveData.tileStates.data()),
            count * sizeof(TileStateData));

        // Save flag
        file.write(reinterpret_cast<char*>(&mapSaveData.hasSavedData), sizeof(bool));

        file.close();
        std::cout << "Map saved: " << count << " tile states for level "
            << static_cast<int>(mapSaveData.savedLevel) << "\n";
    }
    else std::cout << "Failed to save map\n";
}

void SaveManager::LoadMapData()
{
    std::ifstream file("Assets/Saves/mapSave.dat", std::ios::binary);
    if (file.is_open())
    {
        file.read(reinterpret_cast<char*>(&mapSaveData.savedLevel), sizeof(GAME_STATE_TYPE));

        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(size_t));

        mapSaveData.tileStates.resize(count);
        file.read(reinterpret_cast<char*>(mapSaveData.tileStates.data()),
            count * sizeof(TileStateData));

        file.read(reinterpret_cast<char*>(&mapSaveData.hasSavedData), sizeof(bool));

        file.close();
        std::cout << "Map loaded: " << count << " tile states for level "
            << static_cast<int>(mapSaveData.savedLevel) << "\n";
    }
    else std::cout << "Failed to load map\n";
}

void SaveManager::SaveEnemyData()
{
    std::ofstream file("Assets/Saves/enemySave.dat", std::ios::binary);
    if (file.is_open())
    {
        size_t count = enemySaveData.size();
        file.write(reinterpret_cast<char*>(&count), sizeof(size_t));
        file.write(reinterpret_cast<char*>(enemySaveData.data()),
            count * sizeof(EnemySaveData));
        file.close();
        std::cout << "Enemies saved: " << count << "\n";
    }
    else std::cout << "Failed to save enemies\n";
}

void SaveManager::LoadEnemyData()
{
    std::ifstream file("Assets/Saves/enemySave.dat", std::ios::binary);
    if (file.is_open())
    {
        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(size_t));
        enemySaveData.resize(count);
        file.read(reinterpret_cast<char*>(enemySaveData.data()),
            count * sizeof(EnemySaveData));
        file.close();
        std::cout << "Enemies loaded: " << count << "\n";
    }
    else std::cout << "No enemy save found\n";
}

void SaveManager::SaveAll()
{
    // Ask each system to populate save data
    PlayerManager::GetInstance().SavePlayerData();
    EnemyManager::GetInstance().SaveEnemyStates();
    MapManager::GetInstance().SaveMapState();
    

    // Write everything to disk
    SavePlayerData();
    SaveEnemyData();
    SaveMapData();
}
bool SaveManager::HasSaveData()
{
    std::ifstream file("Assets/Saves/playerSave.dat");
    return file.is_open();
}
void SaveManager::LoadAll()
{
    // Read everything from disk first
    LoadPlayerData();
    LoadEnemyData();
    LoadMapData();

    // Apply loaded data to each system
    PlayerManager::GetInstance().Load();
    EnemyManager::GetInstance().LoadEnemyStates();
    MapManager::GetInstance().LoadMapState();
}


void SaveManager::ResetSave()
{
    playerSaveData = PlayerSaveData{};
    mapSaveData = MapSaveData{};
    enemySaveData.clear();
    toContinue = false;
    remove("Assets/Saves/playerSave.dat");
    remove("Assets/Saves/mapSave.dat");
    remove("Assets/Saves/enemySave.dat");
    std::cout << "Save data reset\n";
}
