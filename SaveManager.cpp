#include "SaveManager.h"
#include <string>
#include <fstream>
#include <iostream>

void SaveManager::SavePlayerData(AEVec2 meleePos, AEVec2 rangedPos)
{
    playerSaveData.meleePos = meleePos;
    playerSaveData.rangedPos = rangedPos;
    playerSaveData.hasSavedData = true;

    std::string path = GetSavePath("playerSave.dat");
    std::cout << "Saving to: " << path << '\n';

    std::ofstream file(path, std::ios::binary);
    if (file.is_open())
    {
        file.write(reinterpret_cast<char*>(&playerSaveData), sizeof(PlayerSaveData));
        file.close();
        std::cout << "Saved successfully\n";
    }
    else
    {
        std::cout << "Failed to save to: " << path << '\n';
    }
}

void SaveManager::LoadPlayerData()
{
    std::string path = GetSavePath("playerSave.dat");
    std::cout << "Loading from: " << path << '\n';

    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        file.read(reinterpret_cast<char*>(&playerSaveData), sizeof(PlayerSaveData));
        file.close();
        std::cout << "Loaded successfully\n";
    }
    else
    {
        std::cout << "No save file found at: " << path << '\n';
    }
}

void SaveManager::SetPreservePlayerOnLoad(bool preserve)
{
    playerSaveData.preserveOnLoad = preserve;
}

void SaveManager::SaveMapData()
{
    std::string path = GetSavePath("mapSave.dat");
    std::ofstream file(path, std::ios::binary);
    if (file.is_open())
    {
        size_t count = mapSaveData.tileStates.size();
        file.write(reinterpret_cast<char*>(&count), sizeof(size_t));
        file.write(reinterpret_cast<char*>(mapSaveData.tileStates.data()),
            count * sizeof(TileStateData));
        file.write(reinterpret_cast<char*>(&mapSaveData.hasSavedData), sizeof(bool));
        file.close();
        std::cout << "Map saved: " << count << " tile states\n";
    }
    else std::cout << "Failed to save map\n";
}

void SaveManager::LoadMapData()
{
    std::string path = GetSavePath("mapSave.dat");
    std::ifstream file(path, std::ios::binary);
    if (file.is_open())
    {
        size_t count = 0;
        file.read(reinterpret_cast<char*>(&count), sizeof(size_t));
        mapSaveData.tileStates.resize(count);
        file.read(reinterpret_cast<char*>(mapSaveData.tileStates.data()),
            count * sizeof(TileStateData));
        file.read(reinterpret_cast<char*>(&mapSaveData.hasSavedData), sizeof(bool));
        file.close();
        std::cout << "Map loaded: " << count << " tile states\n";
    }
    else std::cout << "No map save found\n";
}

bool SaveManager::HasSaveData()
{
    return mapSaveData.hasSavedData;
}

void SaveManager::ResetSave()
{
    playerSaveData = PlayerSaveData{};
    mapSaveData = MapSaveData{};
    toContinue = false;
    remove(GetSavePath("playerSave.dat").c_str());
    remove(GetSavePath("mapSave.dat").c_str());
    std::cout << "Save data reset\n";
}