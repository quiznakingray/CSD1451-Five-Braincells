#include "SaveManager.h"
#include <string>
#include <fstream>
#include <iostream>

void SaveManager::SavePlayerData(AEVec2 meleePos, AEVec2 rangedPos)
{
    playerSaveData.meleePos = meleePos;
    playerSaveData.rangedPos = rangedPos;
    playerSaveData.hasSavedData = true;

    std::cout << "Saving to: " << "Assets/Saves/playerSave.dat" << '\n';

    std::ofstream file("Assets/Saves/playerSave.dat", std::ios::binary);
    if (file.is_open())
    {
        file.write(reinterpret_cast<char*>(&playerSaveData), sizeof(PlayerSaveData));
        file.close();
        std::cout << "Saved successfully\n";
    }
    else
    {
        std::cout << "Failed to save to: " << "Assets/Saves/playerSave.dat" << '\n';
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

void SaveManager::SaveMapData()
{
    std::ofstream file("Assets/Saves/mapSave.dat", std::ios::binary);
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
    std::ifstream file("Assets/Saves/mapSave.dat", std::ios::binary);
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
    std::ifstream file("Assets/Saves/playerSave.dat");
    return file.is_open();
}

void SaveManager::ResetSave()
{
    playerSaveData = PlayerSaveData{};
    mapSaveData = MapSaveData{};
    toContinue = false;
    remove("Assets/Saves/playerSave.dat");
    remove("Assets/Saves/mapSave.dat");
    std::cout << "Save data reset\n";
}