//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "MapManager.h"
#include "SpriteManager.h"
#include "PlayerGameObject.h"
#include "PlayerManager.h"
#include "SaveManager.h"
#include "EnemyManager.h"
#include "PlayerStats.h"
#include "Node.h"
#include "EnemyMovement.h"
#include "LoadingScreen.h"
#include "EndMenu.h"
#include <array>
#include <algorithm>
#include <iostream>

#pragma region MapFuncs

rapidcsv::Document map;
std::vector<std::vector<Tile*>> arrMapInfo{};
AEGfxVertexList* mesh;

size_t MapManager::rowCount = 0;
size_t MapManager::colCount = 0;

void MapManager::GenerateNodes()
{
    EnemyMovement::allNodes.clear();

    std::vector<std::vector<Node*>> nodeGrid;
    nodeGrid.resize(rowCount, std::vector<Node*>(colCount, nullptr));

    //------------------------------------------------------------
    // 1. Create Nodes (only on walkable tiles)
    //------------------------------------------------------------
    for (size_t r = 0; r < rowCount; ++r)
    {
        for (size_t c = 0; c < colCount; ++c)
        {
            Tile* tile = arrMapInfo[r][c];

            // Define walkable tiles here
            if (tile->currID != TILE_ID::EMPTY)
            {
                Node* node = new Node();

                node->position = {
                    tile->pos.x,
                    tile->pos.y
                };

                nodeGrid[r][c] = node;
                EnemyMovement::allNodes.push_back(node);
            }
        }
    }

    //------------------------------------------------------------
    // 2. Connect Neighbours (4-directional grid)
    //------------------------------------------------------------
    for (size_t r = 0; r < rowCount; ++r)
    {
        for (size_t c = 0; c < colCount; ++c)
        {
            Node* node = nodeGrid[r][c];
            if (!node) continue;

            // Up
            if (r > 0 && nodeGrid[r - 1][c])
                node->neighbors.push_back(nodeGrid[r - 1][c]);

            // Down
            if (r < rowCount - 1 && nodeGrid[r + 1][c])
                node->neighbors.push_back(nodeGrid[r + 1][c]);

            // Left
            if (c > 0 && nodeGrid[r][c - 1])
                node->neighbors.push_back(nodeGrid[r][c - 1]);

            // Right
            if (c < colCount - 1 && nodeGrid[r][c + 1])
                node->neighbors.push_back(nodeGrid[r][c + 1]);
        }
    }

    std::cout << "[MapManager] Nodes Generated: " << EnemyMovement::allNodes.size() << "\n";
}

void MapManager::InitMap(std::string fileName, GAME_STATE_TYPE currLevel)
{
    AudioManager::GetInstance().PlayMusic("level");
    map = rapidcsv::Document(fileName);
    mapCurrLevel = currLevel;
    // Read a row from the CSV file
    colCount = (map.GetRow<std::string>(0)).size();
    rowCount = (map.GetColumn<std::string>(0)).size();
    arrMapInfo.resize(rowCount);

    for (auto& row : arrMapInfo)
    {
        row.resize(colCount);
    }

    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < colCount; ++uiCol)
        {

            arrMapInfo[uiRow][uiCol] = InitTile(row[uiCol], uiCol, uiRow);
        }
    }
    AEGfxMeshStart();
    AEGfxTriAdd(
        -0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

    AEGfxTriAdd(
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);

    // Saving the mesh (list of triangles) in pMesh
    mesh = AEGfxMeshEnd();

    GenerateNodes();
}

void MapManager::ChangeMap(GAME_STATE_TYPE currLevel)
{
    mapCurrLevel = currLevel;

}

void MapManager::PrintMap() {
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < cCount; ++uiCol)
        {
            std::cout << static_cast<int>(arrMapInfo[uiRow][uiCol]->currID) << ' ';

        }
        std::cout << '\n';
    }
}
void MapManager::LoopMap(void* (mapfunc)())
{    
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < cCount; ++uiCol)
        {
            mapfunc();
        }
        std::cout << '\n';
    }
}
void MapManager::DrawMapSprite()
{
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            Tile * currTile = arrMapInfo[uiRow][uiCol];
            if (currTile->currID != TILE_ID::EMPTY) {
                if (currTile->currID != currTile->bgID && 
                     currTile->isBGActive)
                {
                    currTile->Render();
                }
                if (currTile->isCurrActive) {
                    currTile->Render();
                }
            }
        }
    }
}

void MapManager::FreeMap()
{
    AEGfxMeshFree(mesh);
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t rCount = (map.GetColumn<std::string>(0)).size();
    for (size_t uiRow = 0; uiRow < rCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            Tile* tile = arrMapInfo[uiRow][uiCol];
            // prevents double free 
            if (tile && static_cast<int>(tile->currID) > -1 && static_cast<int>(tile->currID) <= static_cast<int>(TILE_ID::NUM_TILE_ID))
            {
                tile->Free();   // frees all components (sprite, collider, text, etc.)
                delete tile;
                arrMapInfo[uiRow][uiCol] = nullptr;
            }
        }
    }

    for (Node* node : EnemyMovement::allNodes)
    {
        delete node;
    }
    EnemyMovement::allNodes.clear();
    arrMapInfo.clear();
}

void MapManager::SaveMapState()
{
    SaveManager::GetInstance().mapSaveData.tileStates.clear();
    SaveManager::GetInstance().mapSaveData.savedLevel = current;

     for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
        {
            Tile* tile = arrMapInfo[uiRow][uiCol];
            if (!tile) continue;
            if (!tile->canInteract && !dynamic_cast<LaserTile*>(tile) && !dynamic_cast<GateTile*>(tile)) continue;
            if (dynamic_cast<CloudTile*>(tile)) continue;

            TileStateData data;
            data.row = uiRow;
            data.col = uiCol;
            data.currID = tile->currID;
            data.isActive = tile->isActive;
            data.isCurrActive = tile->isCurrActive;
            data.colliderCanCollide = tile->collider ? tile->collider->canCollide : true;
            data.pos = tile->pos;

            if (CheckpointTile* checkpoint = dynamic_cast<CheckpointTile*>(tile))
                data.checkpointActivated = checkpoint->isActivated;

            SaveManager::GetInstance().mapSaveData.tileStates.push_back(data);
        }
    }

    SaveManager::GetInstance().mapSaveData.hasSavedData = true;
}

void MapManager::LoadMapState()
{
    if (!SaveManager::GetInstance().mapSaveData.hasSavedData) return;
    for (const TileStateData& data : SaveManager::GetInstance().mapSaveData.tileStates)
    {
        Tile* tile = arrMapInfo[data.row][data.col];
        if (!tile) continue;
        if (!tile->canInteract && !dynamic_cast<LaserTile*>(tile) && !dynamic_cast<GateTile*>(tile)) continue;
        tile->currID = data.currID;
        tile->isActive = data.isActive;
        tile->isCurrActive = data.isCurrActive;
        tile->pos = data.pos;
        if (tile->collider)
            tile->collider->canCollide = data.colliderCanCollide;
        if (CheckpointTile* checkpoint = dynamic_cast<CheckpointTile*>(tile))
            checkpoint->isActivated = data.checkpointActivated;
        if (LeverTile* lever = dynamic_cast<LeverTile*>(tile))
            lever->SetTexture();
        else if (ButtonTile* button = dynamic_cast<ButtonTile*>(tile))
            button->SetTexture();
    }
}
#pragma endregion

#pragma region TileFuncs
void MapManager::DrawTile(Sprite sprite, AEMtx33 transform)
{
    // Tell the engine to get ready to draw something with texture.
    AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

    // Set the the color to multiply to white, so that the sprite can 
    // display the full range of colors (default is black).
    AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);

    // Set the color to add to nothing, so that we don't alter the sprite's color
    AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

    // Set blend mode to AE_GFX_BM_BLEND
    // This will allow transparency.
    AEGfxSetBlendMode(AE_GFX_BM_BLEND);
    AEGfxSetTransparency(1.0f);

    // Set the texture to pTex
    AEGfxTextureSet(sprite.texture, 0, 0);

        // Tell Alpha Engine to use the matrix in 'transform' to apply onto all
        // the vertices of the mesh that we are about to choose to draw in the next line.
    AEGfxSetTransform(transform.m);

        // Tell Alpha Engine to draw the mesh with the above settings.
    AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

Tile* MapManager::InitTile(std::string cell, size_t col, size_t row)
{
    // saves first int as current currID of tile
    if (cell == "") {
        return nullptr;
    }
    TILE_ID currID = static_cast<TILE_ID>(stoi(cell));
    cell.erase(0, 4);
    TILE_ID bgID = currID;
    int currTag = 0;
    int altTag = 0;
    bool bgActive = true;
    bool currActive = true;

    auto trim = [](std::string s) {
        s.erase(0, s.find_first_not_of(" \t"));
        s.erase(s.find_last_not_of(" \t") + 1);
        return s;
        };

    auto nextToken = [&]() -> std::string {
        size_t pos = cell.find(delimiter);
        std::string token;
        if (pos == std::string::npos) {
            token = trim(cell);
            cell.clear();  // ends the loop
        }
        else {
            token = trim(cell.substr(0, pos));
            cell.erase(0, pos + 1);
        }
        return token;
        };

    bool tagSetAtZero = false;

    int delimitCount = 0;
    while (!cell.empty()) {
        std::string token = nextToken();

        if (delimitCount == 0) {
            if (!token.empty()) {
                int num = stoi(token);
                if (token.length() == 3) {
                    bgID = static_cast<TILE_ID>(num);
                }
                else if (num == 0 || num == 1) {
                    currActive = num;
                }
                else {
                    currTag = num;
                    tagSetAtZero = true;
                }
            }
        }
        else if (delimitCount == 1) {
            if (!token.empty()) {
                if (tagSetAtZero)
                {
                    int num = stoi(token);
                    if (num == 0 || num == 1) {
                        currActive = stoi(token);
                        continue;
                    }
                    altTag = stoi(token);   // currTag already set, this is altTag
                }

                else
                    currTag = stoi(token);  // bgID was at slot 0, this is currTag
            }
        }
        else if (delimitCount == 2) {
            int num = stoi(token);
            if (!token.empty() && (num != 0 && num != 1)) altTag = stoi(token);  // altTag after bgID+currTag case
        }
        else if (delimitCount == 3) {
            if (!token.empty()) currActive = stoi(token);
        }
        else if (delimitCount == 4) {
            if (!token.empty()) bgActive = stoi(token);
        }
        delimitCount++;
    }

    // do checking here
    Tile* newTile{};

    switch (currID)
    {
        // dont do anythign for 
    case TILE_ID::SPIKEDOWN:
    case TILE_ID::SPIKEUP:
    case TILE_ID::SPIKELEFT:
    case TILE_ID::SPIKERIGHT:
        newTile = new SpikeTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;

    case TILE_ID::LEVERREDON:
    case TILE_ID::LEVERREDOFF:
    case TILE_ID::LEVERGREENON:
    case TILE_ID::LEVERGREENOFF:
    case TILE_ID::LEVERBLUEON:
    case TILE_ID::LEVERBLUEOFF:
        newTile = new LeverTile(currID, bgID, currTag, altTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::NOCOLLISIONGROUND:
    case TILE_ID::GRASSCENTER:
    case TILE_ID::GRASSLEFT:
    case TILE_ID::GRASSRIGHT:
    case TILE_ID::GRASSTOP:
    case TILE_ID::GRASSMID:
    case TILE_ID::DIRTCENTER:
    case TILE_ID::DIRTLEFT:
    case TILE_ID::DIRTRIGHT:
    case TILE_ID::DIRTTOP:
    case TILE_ID::DIRTMID:
    case TILE_ID::SANDCENTER:
    case TILE_ID::SANDLEFT:
    case TILE_ID::SANDRIGHT:
    case TILE_ID::SANDTOP:
    case TILE_ID::SANDMID:
        newTile = new GroundTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::WALL:
        newTile = new WallTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;

    case TILE_ID::LASERRED:
    case TILE_ID::LASERGREEN:
    case TILE_ID::LASERBLUE:
        newTile = new LaserTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::CLOUD:
        newTile = new CloudTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::CRATE:
        newTile = new CrateTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::BUTTONBLUETIMEDUNPRESSED:
        newTile = new ButtonTile(currID, bgID, currTag, altTag, bgActive, currActive, row, col, tileSize, true);
        break;
    case TILE_ID::BUTTONBLUEUNPRESSED:
        newTile = new ButtonTile(currID, bgID, currTag, altTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::GATE:
        newTile = new GateTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::GOAL:
        newTile = new GoalTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::CHECKPOINT:
        newTile = new CheckpointTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::HEALTHPICKUPTILE:
        newTile = new HealthPickupTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::DAMAGEPICKUPTILE:
        newTile = new DamagePickupTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::PROFPICKUPTILE:
        newTile = new ProficiencyPickupTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::MOVINGTILEMID:
    case TILE_ID::MOVINGTILELEFT:
    case TILE_ID::MOVINGTILERIGHT:
    case TILE_ID::MOVINGTILEBUTTONMID:
    case TILE_ID::MOVINGTILEBUTTONLEFT:
    case TILE_ID::MOVINGTILEBUTTONRIGHT:
        newTile = new MovingTile(currID, bgID, currTag, altTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::MOVINGTILETARGET:
        newTile = new MovingTileTarget(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    case TILE_ID::HOUSEBEIGEBOTTOMLEFT:
    case TILE_ID::HOUSEBEIGEBOTTOMMID:
    case TILE_ID::HOUSEBEIGEBOTTOMRIGHT:
    case TILE_ID::HOUSEBEIGEMIDLEFT:
    case TILE_ID::HOUSEBEIGEMID:
    case TILE_ID::HOUSEBEIGEMIDRIGHT:
    case TILE_ID::HOUSEBEIGETOPLEFT:
    case TILE_ID::HOUSEBEIGETOPRIGHT:
    case TILE_ID::HOUSEDARKBOTTOMLEFT:
    case TILE_ID::HOUSEDARKBOTTOMMID:
    case TILE_ID::HOUSEDARKBOTTOMRIGHT:
    case TILE_ID::HOUSEDARKMIDLEFT:
    case TILE_ID::HOUSEDARKMID:
    case TILE_ID::HOUSEDARKMIDRIGHT:
    case TILE_ID::HOUSEDARKTOPLEFT:
    case TILE_ID::HOUSEDARKTOPRIGHT:
    case TILE_ID::ROOFYELLOWMID:
    case TILE_ID::ROOFYELLOWLEFT:
    case TILE_ID::ROOFYELLOWRIGHT:
    case TILE_ID::ROOFYELLOWTOPMID:
    case TILE_ID::ROOFYELLOWTOPLEFT:
    case TILE_ID::ROOFYELLOWTOPRIGHT:
    case TILE_ID::ROOFREDMID:
    case TILE_ID::ROOFREDLEFT:
    case TILE_ID::ROOFREDRIGHT:
    case TILE_ID::ROOFREDTOPMID:
    case TILE_ID::ROOFREDTOPLEFT:
    case TILE_ID::ROOFREDTOPRIGHT:
    case TILE_ID::DOORKNOB:
    case TILE_ID::DOORTOP:
    case TILE_ID::WINDOW:
    case TILE_ID::CHIMNEY:
        newTile = new HouseTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
        break;
    default:
        newTile = new Tile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize, false, false, false);
        //newTile->currSprite->texture = SetTileTexture(currID); // can remove this after making structs for all kinds of tiles
        break;
    }

    if (currID == TILE_ID::PLAYER || currID == TILE_ID::EMPTY || currID == TILE_ID::ENEMYMELEE || currID == TILE_ID::EMPTYWITHFIRE) {
        return newTile;
    }

    CheckTileToInit(newTile);

    // if bg tile sprite is present
    if (bgID != currID)
    {
        newTile->bgSprite = newTile->AddComponent(
            new Sprite()
        );
        newTile->bgSprite->textureFileName = GetTileTexture(bgID);

    }

    return newTile;
}

AEGfxTexture* MapManager::SetTileTexture(TILE_ID currID)
{
	AEGfxTexture* tTex;
	switch (currID) 
    {
    case TILE_ID::EMPTY:
    case TILE_ID::PLAYER:
    case TILE_ID::ENEMYMELEE:
        tTex = nullptr;
        break;
    case TILE_ID::GRASSCENTER:
        tTex = AEGfxTextureLoad("Assets/Environment/grassCenter.png");
        break;
    case TILE_ID::WALL:
        tTex = AEGfxTextureLoad("Assets/Environment/wall.png");
        break;
    case TILE_ID::GRASSLEFT:
        tTex = AEGfxTextureLoad("Assets/Environment/grassLeft.png");
        break;
    case TILE_ID::GRASSRIGHT:
        tTex = AEGfxTextureLoad("Assets/Environment/grassRight.png");
        break;
    case TILE_ID::GRASSTOP:
        tTex = AEGfxTextureLoad("Assets/Environment/grassTop.png");
        break;
    case TILE_ID::GRASSMID:
        tTex = AEGfxTextureLoad("Assets/Environment/grassMid.png");
        break;
    case TILE_ID::DIRTCENTER:
        tTex = AEGfxTextureLoad("Assets/Environment/dirtCenter.png");
        break;
    case TILE_ID::DIRTLEFT:
        tTex = AEGfxTextureLoad("Assets/Environment/dirtLeft.png");
        break;
    case TILE_ID::DIRTRIGHT:
        tTex = AEGfxTextureLoad("Assets/Environment/dirtRight.png");
        break;
    case TILE_ID::DIRTTOP:
        tTex = AEGfxTextureLoad("Assets/Environment/dirtTop.png");
        break;
    case TILE_ID::DIRTMID:
        tTex = AEGfxTextureLoad("Assets/Environment/dirtMid.png");
        break;
    case TILE_ID::CLOUD:
        tTex = AEGfxTextureLoad("Assets/Environment/cloudLeft.png");
        break;
    case TILE_ID::GOAL:
        tTex = AEGfxTextureLoad("Assets/Environment/doorClose.png");
        break;
    case TILE_ID::LASERRED:
        tTex = AEGfxTextureLoad("Assets/Environment/laserRedVertical.png");
        break;
    case TILE_ID::LASERGREEN:
        tTex = AEGfxTextureLoad("Assets/Environment/laserGreenVertical.png");
        break;
    case TILE_ID::LASERBLUE:
        tTex = AEGfxTextureLoad("Assets/Environment/laserBlueVertical.png");
        break;
    case TILE_ID::LEVERREDON:
        tTex = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOn.png");
        break;
    case TILE_ID::LEVERREDOFF:
        tTex = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOff.png");
        break;
    case TILE_ID::LEVERGREENON:
        tTex = AEGfxTextureLoad("Assets/Environment/laserGreenSwitchOn.png");
        break;
    case TILE_ID::LEVERGREENOFF:
        tTex = AEGfxTextureLoad("Assets/Environment/laserGreenSwitchOff.png");
        break;
    case TILE_ID::LEVERBLUEON:
        tTex = AEGfxTextureLoad("Assets/Environment/laserBlueSwitchOn.png");
        break;
    case TILE_ID::LEVERBLUEOFF:
        tTex = AEGfxTextureLoad("Assets/Environment/laserBlueSwitchOff.png");
        break;
    case TILE_ID::BUTTONBLUEUNPRESSED:
        tTex = AEGfxTextureLoad("Assets/Environment/buttonBlueUnpressed.png");
        break;
    case TILE_ID::GATE:
        tTex = AEGfxTextureLoad("Assets/Environment/gate.png");
        break;
    case TILE_ID::CHECKPOINT:
        tTex = AEGfxTextureLoad("Assets/Environment/checkpoint.png");
        break;
    case TILE_ID::HEALTHPICKUPTILE:
        tTex = AEGfxTextureLoad("Assets/Environment/gemRed.png");
        break;
    default:
        tTex = AEGfxTextureLoad("Assets/PlanetTexture.png");
        break;
	}
    if (std::find(spikes.begin(), spikes.end(), currID) != spikes.end()) {
        tTex = AEGfxTextureLoad("Assets/Environment/spike.png");
    }
	return tTex;
}
std::string MapManager::GetTileTexture(TILE_ID currID)
{
    std::string tTex;
	switch (currID) 
    {
    case TILE_ID::EMPTY:
    case TILE_ID::PLAYER:
    case TILE_ID::ENEMYMELEE:
        tTex = "";
        break;
    case TILE_ID::GRASSCENTER:
        tTex = "Assets/Environment/grassCenter.png";
        break;
    case TILE_ID::WALL:
        tTex = "Assets/Environment/wall.png";
        break;
    case TILE_ID::GRASSLEFT:
        tTex = "Assets/Environment/grassLeft.png";
        break;
    case TILE_ID::GRASSRIGHT:
        tTex = "Assets/Environment/grassRight.png";
        break;
    case TILE_ID::GRASSTOP:
        tTex = "Assets/Environment/grassTop.png";
        break;
    case TILE_ID::GRASSMID:
        tTex = "Assets/Environment/grassMid.png";
        break;
    case TILE_ID::DIRTCENTER:
        tTex = "Assets/Environment/dirtCenter.png";
        break;
    case TILE_ID::DIRTLEFT:
        tTex = "Assets/Environment/dirtLeft.png";
        break;
    case TILE_ID::DIRTRIGHT:
        tTex = "Assets/Environment/dirtRight.png";
        break;
    case TILE_ID::DIRTTOP:
        tTex = "Assets/Environment/dirtTop.png";
        break;
    case TILE_ID::DIRTMID:
        tTex = "Assets/Environment/dirtMid.png";
        break;
    case TILE_ID::CLOUD:
        tTex = "Assets/Environment/cloud.png";
        break;
    case TILE_ID::GOAL:
        tTex = "Assets/Environment/doorClose.png";
        break;
    case TILE_ID::LASERRED:
        tTex = "Assets/Environment/laserRedVertical.png";
        break;
    case TILE_ID::LASERGREEN:
        tTex = "Assets/Environment/laserGreenVertical.png";
        break;
    case TILE_ID::LASERBLUE:
        tTex = "Assets/Environment/laserBlueVertical.png";
        break;
    case TILE_ID::LEVERREDON:
        tTex = "Assets/Environment/laserRedSwitchOn.png";
        break;
    case TILE_ID::LEVERREDOFF:
        tTex = "Assets/Environment/laserRedSwitchOff.png";
        break;
    case TILE_ID::LEVERGREENON:
        tTex = "Assets/Environment/laserGreenSwitchOn.png";
        break;
    case TILE_ID::LEVERGREENOFF:
        tTex = "Assets/Environment/laserGreenSwitchOff.png";
        break;
    case TILE_ID::LEVERBLUEON:
        tTex = "Assets/Environment/laserBlueSwitchOn.png";
        break;
    case TILE_ID::LEVERBLUEOFF:
        tTex = "Assets/Environment/laserBlueSwitchOff.png";
        break;
    case TILE_ID::BUTTONBLUEUNPRESSED:
        tTex = "Assets/Environment/buttonBlueUnpressed.png";
        break;
    case TILE_ID::GATE:
        tTex = "Assets/Environment/gate.png";
        break;
    case TILE_ID::CHECKPOINT:
        tTex = "Assets/Environment/checkpoint.png";
        break;
    case TILE_ID::HEALTHPICKUPTILE:
        tTex = "Assets/Environment/gemRed.png";
        break;
    case TILE_ID::HOUSEBEIGEBOTTOMLEFT:
        tTex = "Assets/Environment/houseBeigeBottomLeft.png";
        break;
    case TILE_ID::HOUSEBEIGEBOTTOMMID:
        tTex = "Assets/Environment/houseBeigeBottomMid.png";
        break;
    case TILE_ID::HOUSEBEIGEBOTTOMRIGHT:
        tTex = "Assets/Environment/houseBeigeBottomRight.png";
        break;
    case TILE_ID::HOUSEBEIGEMIDLEFT:
        tTex = "Assets/Environment/houseBeigeMidLeft.png";
        break;
    case TILE_ID::HOUSEBEIGEMID:
        tTex = "Assets/Environment/houseBeigeMid.png";
        break;
    case TILE_ID::HOUSEBEIGEMIDRIGHT:
        tTex = "Assets/Environment/houseBeigeMidRight.png";
        break;
    case TILE_ID::HOUSEBEIGETOPLEFT:
        tTex = "Assets/Environment/houseBeigeTopLeft.png";
        break;
    case TILE_ID::HOUSEBEIGETOPRIGHT:
        tTex = "Assets/Environment/houseBeigeTopRight.png";
        break;
    case TILE_ID::HOUSEDARKBOTTOMLEFT:
        tTex = "Assets/Environment/houseDarkBottomLeft.png";
        break;
    case TILE_ID::HOUSEDARKBOTTOMMID:
        tTex = "Assets/Environment/houseDarkBottomMid.png";
        break;
    case TILE_ID::HOUSEDARKBOTTOMRIGHT:
        tTex = "Assets/Environment/houseDarkBottomRight.png";
        break;
    case TILE_ID::HOUSEDARKMIDLEFT:
        tTex = "Assets/Environment/houseDarkMidLeft.png";
        break;
    case TILE_ID::HOUSEDARKMID:
        tTex = "Assets/Environment/houseDarkMid.png";
        break;
    case TILE_ID::HOUSEDARKMIDRIGHT:
        tTex = "Assets/Environment/houseDarkMidRight.png";
        break;
    case TILE_ID::HOUSEDARKTOPLEFT:
        tTex = "Assets/Environment/houseDarkTopLeft.png";
        break;
    case TILE_ID::HOUSEDARKTOPRIGHT:
        tTex = "Assets/Environment/houseDarkTopRight.png";
        break;
    case TILE_ID::ROOFYELLOWMID:
        tTex = "Assets/Environment/roofYellowMid.png";
        break;
    case TILE_ID::ROOFYELLOWLEFT:
        tTex = "Assets/Environment/roofYellowLeft.png";
        break;
    case TILE_ID::ROOFYELLOWRIGHT:
        tTex = "Assets/Environment/roofYellowRight.png";
        break;
    case TILE_ID::ROOFYELLOWTOPMID:
        tTex = "Assets/Environment/roofYellowTopMid.png";
        break;
    case TILE_ID::ROOFYELLOWTOPLEFT:
        tTex = "Assets/Environment/roofYellowTopLeft.png";
        break;
    case TILE_ID::ROOFYELLOWTOPRIGHT:
        tTex = "Assets/Environment/roofYellowTopRight.png";
        break;
    case TILE_ID::ROOFREDMID:
        tTex = "Assets/Environment/roofRedMid.png";
        break;
    case TILE_ID::ROOFREDLEFT:
        tTex = "Assets/Environment/roofRedLeft.png";
        break;
    case TILE_ID::ROOFREDRIGHT:
        tTex = "Assets/Environment/roofRedRight.png";
        break;
    case TILE_ID::ROOFREDTOPMID:
        tTex = "Assets/Environment/roofRedTopMid.png";
        break;
    case TILE_ID::ROOFREDTOPLEFT:
        tTex = "Assets/Environment/roofRedTopLeft.png";
        break;
    case TILE_ID::ROOFREDTOPRIGHT:
        tTex = "Assets/Environment/roofRedTopRight.png";
        break;
    case TILE_ID::DOORKNOB:
        tTex = "Assets/Environment/doorKnob.png";
        break;
    case TILE_ID::DOORTOP:
        tTex = "Assets/Environment/doorTop.png";
        break;
    case TILE_ID::WINDOW:
        tTex = "Assets/Environment/window.png";
        break;
    case TILE_ID::CHIMNEY:
        tTex = "Assets/Environment/chimney.png";
        break;
    default:
        tTex = "Assets/PlanetTexture.png";
        break;
	}
    if (std::find(spikes.begin(), spikes.end(), currID) != spikes.end()) {
        tTex = "Assets/Environment/spike.png";
    }
	return tTex;
}

void MapManager::CheckTileToInit(Tile* tile)
{
    if ((std::find(spikes.begin(), spikes.end(), tile->currID) != spikes.end()))
    {
        switch (tile->currID) {
        case TILE_ID::SPIKEDOWN:
        {
            tile->rotation = 0;
            break;
        }
        case TILE_ID::SPIKEUP:
        {
            tile->rotation = PI;
            break;
        }
        case TILE_ID::SPIKELEFT:
        {
            tile->rotation = 270 * (PI / 180);
            break;
        }
        case TILE_ID::SPIKERIGHT:
        {
            tile->rotation = PI / 2;
            break;
        }
        }
    }
}

std::vector<Tile*> MapManager::GetTilesWithID(TILE_ID currID)
{
    size_t rCount = (map.GetRow<std::string>(0)).size();
    size_t cCount = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> foundTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < rCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            if (arrMapInfo[uiCol][uiRow]->currID == currID)
            {
                foundTiles.push_back(arrMapInfo[uiCol][uiRow]);
                i++;
            }

        }
    }
    return foundTiles;
}

Tile* MapManager::GetTile(size_t col, size_t row)
{
    if (arrMapInfo.size() <= 0) return nullptr;

    return arrMapInfo[col][row];
}
std::vector<Tile*> MapManager::GetTaggedTiles(int tag)
{
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t rCount = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> taggedTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < rCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            if (arrMapInfo[uiCol][uiRow]->currTag == tag)
            {
                taggedTiles.push_back(arrMapInfo[uiCol][uiRow]);
                i++;
            }

        }
    }
    return taggedTiles;
    
}

std::vector<Tile*> MapManager::GetTaggedTiles(int tag, TILE_ID id)
{
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t rCount = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> taggedTiles;

    for (size_t uiRow = 0; uiRow < rCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            Tile* currTile = arrMapInfo[uiRow][uiCol];
            if (currTile->currTag == tag && 
                currTile->currID == id)
            {
                taggedTiles.push_back(currTile);
                i++;
            }

        }
    }
    return taggedTiles;
}
std::vector<Tile*> MapManager::GetAltTaggedTiles(int altTag, TILE_ID id)
{
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t rCount = (map.GetColumn<std::string>(0)).size();
    std::vector<Tile*> taggedTiles;

    for (size_t uiRow = 0; uiRow < rCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            Tile* currTile = arrMapInfo[uiRow][uiCol];
            if (currTile->altTag == altTag &&
                currTile->currID == id)
            {
                taggedTiles.push_back(currTile);
            }
        }
    }
    return taggedTiles;
}

#pragma endregion



#pragma region LaserFuncs
void MapManager::SetLaserActive(Tile tile, bool active)
{
    std::vector<Tile*> lasers = GetTaggedTiles(tile.currTag, tile.currID);
    for (Tile* laser : lasers) {
        laser->isCurrActive = active;
    }
    AudioManager::GetInstance().PlaySFX("laserOn");
}
#pragma endregion



#pragma region GetFuncs
std::vector<Tile*> MapManager::GetTilesNearPos(AEVec2 pos, AEVec2 scale)
{
    std::vector<Tile*> nearbyTiles;

    for (std::vector<Tile*> row : arrMapInfo)
    {
        // Load a particular CSV value into the arrMapInfo
        for (Tile* tile : row)
        {
            //Tile* currTile = arrMapInfo[mapCurrLevel][uiCol][uiRow];
            if (BoxToBoxCollision(pos, tile->pos, scale, tile->scale) && tile->currID != TILE_ID::PLAYER)
            {
                nearbyTiles.push_back(tile);
            }
        }
    }
    return nearbyTiles;
}

Tile* MapManager::GetTile(TILE_ID id)
{
    for (std::vector<Tile*> row : arrMapInfo)
    {
        // Load a particular CSV value into the arrMapInfo
        for (Tile* tile : row)
        {
            //Tile *currTile = arrMapInfo[mapCurrLevel][uiRow][uiCol];
            if (tile->currID == id) {
                return tile;
            }
        }
    }
    return nullptr;
}
AEVec2 MapManager::GetPlayerSpawnPos()
{
    AEVec2 pos;
    AEVec2Set(&pos, 0, 0);

    Tile* player = GetTile(TILE_ID::PLAYER);
    AEVec2Set(&pos, player->pos.x, player->pos.y);
    return pos;
}

size_t MapManager::GetRow()
{
    return  map.GetRowCount();
}

size_t MapManager::GetCol()
{
    return map.GetColumnCount();
}

void MapManager::AddTilesToGameObjectVector(std::vector<GameObject*>& gos)
{
    size_t cCount = (map.GetRow<std::string>(0)).size();
    size_t rCount = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < rCount; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < cCount; uiCol++)
        {
            Tile* currTile = arrMapInfo[uiRow][uiCol];

            if (currTile->currID != TILE_ID::EMPTY) AddGameObjectToVector(currTile, gos);
        }
    }
}
#pragma endregion

Tile::~Tile()
{
    if (currSprite) {
        currSprite = nullptr;
    }
    if (bgSprite) {
        bgSprite = nullptr;
    }
    if (collider) {
        collider = nullptr;
    }
    if (interactionTextBox) {
        interactionTextBox = nullptr;
    }

}

void Tile::Update()
{
    GameObject::Update();
}

void SpikeTile::Init() {
    Tile::Init();
    //collider->isTrigger = true;
    collider->size.x = 0.7f;
    collider->size.y = 0.7f;
    collider->OnCollisionEnter = [this](Collider* other, int) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
        {
            if (!other->isTrigger) {
                player->TakeDamage(1);
                // knockback based on collision side
                RigidBody* playerRb = player->GetComponent<RigidBody>();
                float knockbackX = 300.0f;
                float knockbackY = 500.0f;

                switch (static_cast<int>(currID))
                {
                case static_cast<int>(TILE_ID::SPIKEUP):
                    playerRb->velocity.y = -knockbackY;
                    break;
                case static_cast<int>(TILE_ID::SPIKEDOWN):
                    playerRb->velocity.y = knockbackY;
                    break;
                case static_cast<int>(TILE_ID::SPIKELEFT):
                    playerRb->velocity.x = knockbackX;
                    break;
                case static_cast<int>(TILE_ID::SPIKERIGHT):
                    playerRb->velocity.x = -knockbackX;
                    break;
                default:
                    break;
                }
            }
            
        }
        };
}

void HealthPickupTile::Init() {
    Tile::Init();
    collider->isTrigger = true;
    collider->OnTriggerEnter = [this](Collider* other, int) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player && !other->isTrigger && PlayerStats::GetInstance().GetPlayerHealth() < PlayerStats::GetInstance().GetPlayerMaxHealth())
        {
            AudioManager::GetInstance().PlaySFX("itemPickup");
            PlayerStats::GetInstance().IncreasePlayerHealth();
            isCurrActive = false;
            collider->canCollide = false;
        }
        else if (PlayerStats::GetInstance().GetPlayerHealth() >= PlayerStats::GetInstance().GetPlayerMaxHealth()) {
            interactionTextBox->isActive = true;
            interactionTextBox->SetText("Health at max!");
        }
        };
    collider->OnTriggerExit = [this](Collider* other, int) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
            this->interactionTextBox->isActive = false;
        };
}

void DamagePickupTile::Init() {
    Tile::Init();
    collider->isTrigger = true;
    collider->OnTriggerEnter = [this](Collider* other, int) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player && !other->isTrigger && isCurrActive) {
            AudioManager::GetInstance().PlaySFX("itemPickup");
            PlayerStats::GetInstance().IncreasePlayerDamage();
            isCurrActive = false;
            collider->canCollide = false;
        }
        };
}

void ProficiencyPickupTile::Init() {
    Tile::Init();
    collider->isTrigger = true;
    collider->OnTriggerEnter = [this](Collider* other, int) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player && !other->isTrigger && isCurrActive) {
            AudioManager::GetInstance().PlaySFX("itemPickup");
            PlayerStats::GetInstance().IncreasePlayerProficiency(proficiencyAmount);
            isCurrActive = false;
            collider->canCollide = false;
        }
        };
}


void GoalTile::Init() {
    Tile::Init();
    collider->center.y = 0.5f;
    collider->size.x = 2.f;
    collider->size.y = 1.5f;
    collider->isTrigger = true;

    collider->OnTriggerEnter = [this](Collider* other, int) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
            this->interactionTextBox->isActive = true;
    };

    collider->OnTriggerOver = [this](Collider* other, int) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
        {
            interactionTextBox->SetText("[F] Enter");
            if (AEInputCheckTriggered(AEVK_F))
            {
                AudioManager::GetInstance().PlaySFX("goalEnter");
                // transition to next level based on current
                switch (current)
                {
                case GAME_STATE_TYPE::LEVEL1:
                    LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL2;
                    GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
                    break;
                case GAME_STATE_TYPE::LEVEL2:
                    LoadingScreen::targetState = GAME_STATE_TYPE::LEVEL3;
                    GameStateManager::GetInstance().ChangeState(GAME_STATE_TYPE::LOADING);
                    break;
                case GAME_STATE_TYPE::LEVEL3:
                    // put end menu here
					EndMenu::GetInstance().won = true;
					EndMenu::GetInstance().isActive = true;
					GameStateManager::GetInstance().gamePaused = true;
                    break;
                default:
                    next = GAME_STATE_TYPE::LEVEL1;
                    break;
                }
                SaveManager::GetInstance().toContinue = false;
            }
        }
    };

    collider->OnTriggerExit = [this](Collider* other, int) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
            this->interactionTextBox->isActive = false;
        };
}

void CheckpointTile::Init() {
    Tile::Init();
    checkpointParticles.Init(5, true);
    canArrowPass = true;
    collider->isTrigger = true;
    collider->OnTriggerEnter = [this](Collider* other, int) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player)
        {
            if (!isActivated)
            {
                isActivated = true;
                AudioManager::GetInstance().PlaySFX("checkpoint");
                checkpointParticles.CreateCheckpointExplosion(pos.x, pos.y);
            }
            this->interactionTextBox->isActive = true;
            interactionTextBox->SetText("Saved!");
            SaveManager::GetInstance().SaveAll();
        }
        };
    collider->OnTriggerExit = [this](Collider* other, int) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
        {
            this->interactionTextBox->isActive = false;
        }
        };
}

void CheckpointTile::Update()
{
	float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
    Tile::Update();
	checkpointParticles.Update(dt);
}

void CheckpointTile::Render()
{
    Tile::Render();
    if (isActivated)
		checkpointParticles.Draw();
}

void CheckpointTile::Free()
{
    Tile::Free();
	checkpointParticles.Exit();
}

void CrateTile::Init()
{
    Tile::Init();

    collider->size.x = 0.875f;
    collider->size.y = 0.875f;

    interactionTextBox->SetText("[F] Grab");

    collider->OnCollisionEnter = [this](Collider* other, int) {

        int hitSides = collider->GetSidesForCollider(other);
        if (hitSides & COLLISION_SIDE::BOTTOM) {
            if (isOnCamera)
                AudioManager::GetInstance().PlaySFX("crateLanding");

            // Check if crate landed on an enemy while falling
            EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(other->owner);
            if (enemy && enemy->base.isAlive && rb->velocity.y < 0.f) {
                if (enemy->base.type == EnemyType::BASIC_MELEE ||
                    enemy->base.type == EnemyType::BASIC_RANGED)
                {
                    AudioManager::GetInstance().PlaySFX("enemyDie");
                }
                else {
                    AudioManager::GetInstance().PlaySFX("minibossDie");
                }
                enemy->base.stats.health = 0;
                enemy->base.isAlive = false;
            }
        }
        Player* player = dynamic_cast<Player*>(other->owner);
        if (PlayerManager::GetInstance().currentPlayer != player)
            return;
        if (player && !pushState)
            interactionTextBox->isActive = true;

        Arrow* arrow = dynamic_cast<Arrow*>(other->owner);
        if (arrow && arrow->isActive)
        {

            if (hitSides & COLLISION_SIDE::LEFT || hitSides & COLLISION_SIDE::RIGHT)
            {
                rb->velocity.x = (hitSides & COLLISION_SIDE::LEFT) ? 200.0f : -200.0f;
            }
            arrow->isActive = false;
            arrow->timer = 0.0f;
        }
        };
    collider->OnCollisionOver = [this](Collider* other, int) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player)
        {
            if (PlayerManager::GetInstance().currentPlayer != player)
                return;
            if (AEInputCheckTriggered(AEVK_F)) {
                pushState = !pushState;
                if (pushState) {
                    grabbedPlayer = player;
                    grabbedPlayer->currentAction = PLAYER_ACTION::CRATEINTERACT;
                    grabbedSide = playerOnLeft ? COLLISION_SIDE::LEFT : COLLISION_SIDE::RIGHT;
                    interactionTextBox->isActive = false;
                    AudioManager::GetInstance().PlaySFX("cratePickUp");
                }
                else {
                    grabbedPlayer->currentAction = PLAYER_ACTION::IDLE;
                    grabbedPlayer = nullptr;
                    grabbedSide = 0;
                    interactionTextBox->isActive = true;
                }
            }
        }
        };

    collider->OnCollisionExit = [this](Collider* other, int) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player && !pushState)
        {
            playerTouching = false;
            playerOnLeft = false;
            playerOnRight = false;
            grabbedPlayer = nullptr;
            interactionTextBox->isActive = false;
        }
        };
}

void CrateTile::Update() {
    Tile::Update();
    double dt = AEFrameRateControllerGetFrameTime();

    std::vector<Tile*> nearbyTiles = MapManager::GetTilesNearPos(pos, scale);
    std::vector<Collider*> colliders = GetComponents<Collider>();

    // cancel grab if player is too far
    if (pushState && grabbedPlayer)
    {
        float dist = AEVec2Distance(&pos, &grabbedPlayer->pos);
        if (dist > scale.x * 2.0f)
        {
            pushState = false;
            grabbedPlayer->currentAction = PLAYER_ACTION::IDLE;
            grabbedPlayer = nullptr;
            grabbedSide = 0;
        }
    }

    if (pushState && grabbedPlayer && grabbedPlayer->currentAction == PLAYER_ACTION::CRATEINTERACT)
    {
        // release grab when F is pressed
        if (AEInputCheckTriggered(AEVK_F))
        {
            pushState = false;
            grabbedPlayer->currentAction = PLAYER_ACTION::IDLE;
            grabbedPlayer = nullptr;
            grabbedSide = 0;
            interactionTextBox->isActive = false;
            return;
        }
        // only update grabbed side if player is moving fast enough
        if (grabbedPlayer->rb->velocity.x > 1.0f)
            grabbedSide = COLLISION_SIDE::RIGHT;
        else if (grabbedPlayer->rb->velocity.x < -1.0f)
            grabbedSide = COLLISION_SIDE::LEFT;

        // sync crate to player
        if (grabbedSide == COLLISION_SIDE::RIGHT)
            pos.x = grabbedPlayer->pos.x + grabbedPlayer->scale.x * 0.5f + scale.x * 0.5f;
        else if (grabbedSide == COLLISION_SIDE::LEFT)
            pos.x = grabbedPlayer->pos.x - grabbedPlayer->scale.x * 0.5f - scale.x * 0.5f;

        pos.y = grabbedPlayer->pos.y;
        rb->velocity.x = 0;
        rb->velocity.y = 0;
        rb->onCollider = grabbedPlayer->rb->onCollider;
        // wall clip check AFTER sync
        for (Tile* tile : nearbyTiles)
        {
            if (!tile) continue;
            if (tile == this) continue;
            if (!tile->collider || !tile->collider->canCollide) continue;
            if (tile->collider->isTrigger) continue;
            if (dynamic_cast<LeverTile*>(tile)) continue;

            Collider* oCol = tile->collider;
            if (!oCol) continue;
            if (BoxToBoxCollision(
                collider->GetPos2D(), oCol->GetPos2D(),
                collider->GetScale(), oCol->GetScale()))
            {
                if (oCol->isTrigger) {
                    collider->AddToOvelappingVector(oCol, 0); // 0 or COLLISION_SIDE::NONE
                    oCol->AddToOvelappingVector(collider, 0);
                    continue;
                }

                float dx = collider->GetPos2D().x - oCol->GetPos2D().x;
                float dy = collider->GetPos2D().y - oCol->GetPos2D().y;
                float pxOverlap = (collider->GetScale().x * 0.5f + oCol->GetScale().x * 0.5f) - fabs(dx);
                float pyOverlap = (collider->GetScale().y * 0.5f + oCol->GetScale().y * 0.5f) - fabs(dy);
                if (pxOverlap < pyOverlap)
                {
                    // push crate out of wall
                    pos.x += (dx > 0) ? pxOverlap : -pxOverlap;

                    // sync player back to crate
                    if (grabbedSide == COLLISION_SIDE::RIGHT)
                        grabbedPlayer->pos.x = pos.x - scale.x * 0.5f - grabbedPlayer->scale.x * 0.5f;
                    else if (grabbedSide == COLLISION_SIDE::LEFT)
                        grabbedPlayer->pos.x = pos.x + scale.x * 0.5f + grabbedPlayer->scale.x * 0.5f;

                    if (RigidBody* playerRb = grabbedPlayer->GetComponent<RigidBody>())
                        playerRb->velocity.x = 0;
                }
                else
                {
                    pos.y += (dy > 0) ? pyOverlap : -pyOverlap;
                }
            }
        }
    }
    else
    {
        bool wasOnCollider = rb->onCollider;
        PhysicsManager::UpdateRigidBody(rb, static_cast<f32>(dt));

        // if crate just left the ground, bleed off any residual X nudge velocity
        if (wasOnCollider && !rb->onCollider)
            rb->velocity.x = 0.f;

        rb->onCollider = false;
        PhysicsManager::UpdateRigidBody(rb, static_cast<f32>(dt));

        // friction when not grabbed
        if (!playerTouching)
        {
            if (rb->velocity.x > 0) {
                rb->velocity.x -= static_cast<f32>(friction * dt);
                if (rb->velocity.x < 0) rb->velocity.x = 0;
            }
            else if (rb->velocity.x < 0) {
                rb->velocity.x += static_cast<f32>(friction * dt);
                if (rb->velocity.x > 0) rb->velocity.x = 0;
            }
        }
        for (Collider* pCol : colliders)
        {
            playerTouching = false;
            playerOnLeft = false;
            playerOnRight = false;

            for (const auto& info : pCol->collisionInfos)
            {
                Collider* other = info.other;
                if (!other || !other->owner) continue;

                Player* player = dynamic_cast<Player*>(other->owner);
                if (player && !playerTouching)
                {
                    playerTouching = true;
                    if (info.sides & COLLISION_SIDE::LEFT)  playerOnLeft = true;
                    if (info.sides & COLLISION_SIDE::RIGHT) playerOnRight = true;
                }
            }
            for (Tile* tile : nearbyTiles)
            {
                if (!tile) continue;
                if (tile == this) continue;
                if (!tile->collider || !tile->collider->canCollide) continue;
                if (dynamic_cast<LeverTile*>(tile)) continue;

                Collider* oCol = tile->collider;
                if (!oCol) continue;
                if (BoxToBoxCollision(
                    pCol->GetPos2D(), oCol->GetPos2D(),
                    pCol->GetScale(), oCol->GetScale()))
                {
                    int sidesForCrate = GetAllCollisionSides(
                        pCol->GetPos2D(), oCol->GetPos2D(),
                        pCol->GetScale(), oCol->GetScale()
                    );
                    int sidesForTile = FlipCollisionSides(sidesForCrate);

                    if (sidesForCrate & COLLISION_SIDE::BOTTOM)
                        rb->onCollider = true;

                    pCol->AddToOvelappingVector(oCol, sidesForCrate);
                    oCol->AddToOvelappingVector(pCol, sidesForTile);
                    PhysicsManager::HandleCollision(pCol, oCol);
                }
                else
                {
                    pCol->RemoveFromOverlappingVector(oCol);
                    oCol->RemoveFromOverlappingVector(pCol);
                }
            }
            for (auto it = pCol->collisionInfos.begin(); it != pCol->collisionInfos.end(); )
            {
                Collider* oCol = it->other;
                if (!oCol || !oCol->canCollide)
                {
                    pCol->RemoveFromOverlappingVector(oCol);
                    it = pCol->collisionInfos.begin();
                    continue;
                }
                if (oCol->owner)
                {
                    EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(oCol->owner);
                    if (enemy && !enemy->base.isAlive)
                    {
                        pCol->RemoveFromOverlappingVector(oCol);
                        it = pCol->collisionInfos.begin();
                        continue;
                    }
                }
                if (BoxToBoxCollision(
                    pCol->GetPos2D(), oCol->GetPos2D(),
                    pCol->GetScale(), oCol->GetScale()))
                {
                    PhysicsManager::HandleCollision(pCol, oCol);
                    ++it;
                }
                else
                {
                    pCol->RemoveFromOverlappingVector(oCol);
                    it = pCol->collisionInfos.begin();
                }
            }
        }
    }
}