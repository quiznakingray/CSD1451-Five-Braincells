#include "MapManager.h"
#include "SpriteManager.h"
#include "PlayerGameObject.h"
#include "PlayerManager.h"
#include "SaveManager.h"
#include "EnemyManager.h"
#include <array>
#include <algorithm>
#include <iostream>

#pragma region MapFuncs

rapidcsv::Document map;
std::vector<std::vector<Tile*>> arrMapInfo{};
AEGfxVertexList* mesh;

size_t MapManager::rowCount = 0;
size_t MapManager::colCount = 0;

GAME_STATE_TYPE MapManager::mapCurrLevel = GAME_STATE_TYPE::LEVEL1;

void MapManager::InitMap(std::string fileName, GAME_STATE_TYPE currLevel)
{
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
}

void MapManager::ChangeMap(GAME_STATE_TYPE currLevel)
{
    mapCurrLevel = currLevel;

}

void MapManager::PrintMap() {
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < colCount; ++uiCol)
        {
            std::cout << static_cast<int>(arrMapInfo[uiRow][uiCol]->currID) << ' ';

        }
        std::cout << '\n';
    }
}
void MapManager::LoopMap(void* (mapfunc)())
{    
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < colCount; ++uiCol)
        {
            mapfunc();
        }
        std::cout << '\n';
    }
}
void MapManager::DrawMapSprite()
{
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
        {
            Tile * currTile = arrMapInfo[uiRow][uiCol];
            if (currTile->currID != TILE_ID::EMPTY) {
                if (currTile->currID != currTile->bgID && 
                     currTile->isBGActive)
                {
                    //RenderSprite(currTile->bgSprite, mesh);
                    currTile->Render();
                }
                if (currTile->isCurrActive) {
                    //RenderSprite(*currTile.currSprite, mesh);
                    //currTile.currSprite->Render();
                    currTile->Render();
                }
            }
        }
    }
}

void MapManager::FreeMap()
{
    AEGfxMeshFree(mesh);
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t rowCount = (map.GetColumn<std::string>(0)).size();
    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
        {
            Tile* tile = arrMapInfo[uiRow][uiCol];
            // prevents double free 
            if (tile && static_cast<int>(tile->currID) > -1 && static_cast<int>(tile->currID) <= 300)
            {
                tile->Free();   // frees all components (sprite, collider, text, etc.)
                delete tile;
            }
        }
    }
}

void MapManager::SaveMapState()
{
    SaveManager::GetInstance().mapSaveData.tileStates.clear();
    SaveManager::GetInstance().mapSaveData.savedLevel = mapCurrLevel;

    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
        {
            Tile* tile = arrMapInfo[uiRow][uiCol];
            if (!tile) continue;

            bool isInteractable =
                dynamic_cast<LeverTile*>(tile) ||
                dynamic_cast<ButtonTile*>(tile) ||
                dynamic_cast<CrateTile*>(tile) ||
                dynamic_cast<LaserTile*>(tile) ||
                dynamic_cast<GateTile*>(tile) ||
                dynamic_cast<CloudTile*>(tile) ||
                dynamic_cast<HealthPickupTile*>(tile);

            if (!isInteractable) continue;

            TileStateData data;
            data.row = uiRow;
            data.col = uiCol;
            data.currID = tile->currID;
            data.isActive = tile->isActive;
            data.isCurrActive = tile->isCurrActive;
            data.colliderCanCollide = tile->collider ? tile->collider->canCollide : true;
            data.pos = tile->pos;

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
        tile->currID = data.currID;
        tile->isActive = data.isActive;
        tile->isCurrActive = data.isCurrActive;
        tile->pos = data.pos;
        if (tile->collider)
            tile->collider->canCollide = data.colliderCanCollide;
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
    default:
        newTile = new Tile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize, true);
        newTile->currSprite->texture = SetTileTexture(currID); // can remove this after making structs for all kinds of tiles
        break;
    }

    if (currID == TILE_ID::PLAYER || currID == TILE_ID::EMPTY || currID == TILE_ID::ENEMY) {
        return newTile;
    }

    CheckTileToInit(newTile);

    // if bg tile sprite is present
    if (bgID != currID)
    {
        newTile->bgSprite = newTile->AddComponent(
            new Sprite()
        );
        newTile->bgSprite->texture = SetTileTexture(bgID);

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
    case TILE_ID::ENEMY:
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

void MapManager::RotateTile(double rotation, Tile tile)
{

}
std::vector<Tile*> MapManager::GetTilesWithID(TILE_ID currID)
{
    size_t rowCount = (map.GetRow<std::string>(0)).size();
    size_t colCount = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> foundTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
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

Tile* MapManager::GetTile(unsigned int col, unsigned int row)
{
    return arrMapInfo[col][row];
}
std::vector<Tile*> MapManager::GetTaggedTiles(int tag)
{
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t rowCount = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> taggedTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
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
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t rowCount = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> taggedTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
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


#pragma endregion



#pragma region LaserFuncs
void MapManager::SetLaserActive(Tile tile, bool active)
{
    std::vector<Tile*> lasers = GetTaggedTiles(tile.currTag, tile.currID);
    for (Tile* laser : lasers) {
        laser->isCurrActive = active;
    }
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

void MapManager::AddTilesToGameObjectVector(std::vector<GameObject*>& gos)
{
    size_t colCount = (map.GetRow<std::string>(0)).size();
    size_t rowCount = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < rowCount; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < colCount; uiCol++)
        {
            Tile* currTile = arrMapInfo[uiRow][uiCol];

            if (currTile->currID != TILE_ID::EMPTY) AddGameObjectToVector(currTile, gos);
        }
    }
}
#pragma endregion

void Tile::Update()
{
    GameObject::Update();
}

void GoalTile::Init() {
    Tile::Init();
    collider->center.y = 0.5f;
    collider->size.x = 2.f;
    collider->size.y = 1.5f;
    collider->isTrigger = true;

    collider->OnTriggerEnter = [this](Collider* other, int sides) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
            this->interactionTextBox->isActive = true;
    };

    collider->OnTriggerOver = [this](Collider* other, int sides) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
        {
            interactionTextBox->text = "[F] Enter";
            if (AEInputCheckTriggered(AEVK_F))
            {
                SaveManager::GetInstance().SaveAll();
                //// save current state before transitioning
                //SaveManager::GetInstance().SavePlayerData(
                //    PlayerManager::GetInstance().meleePlayer->pos,
                //    PlayerManager::GetInstance().rangedPlayer->pos
                //);
                //MapManager::GetInstance().SaveMapState(current);
                

                // transition to next level based on current
                switch (current)
                {
                case GAME_STATE_TYPE::LEVEL1:
                    SaveManager::GetInstance().SetPreservePlayerOnLoad(true);
                    next = GAME_STATE_TYPE::LEVEL1BOSS;
                    break;
                case GAME_STATE_TYPE::LEVEL1BOSS:
                    SaveManager::GetInstance().SetPreservePlayerOnLoad(false);
                    next = GAME_STATE_TYPE::LEVEL2;
                    break;
                case GAME_STATE_TYPE::LEVEL2:
                    SaveManager::GetInstance().SetPreservePlayerOnLoad(true);
                    next = GAME_STATE_TYPE::LEVEL2BOSS;
                    break;
                case GAME_STATE_TYPE::LEVEL2BOSS:
                    SaveManager::GetInstance().SetPreservePlayerOnLoad(false);
                    next = GAME_STATE_TYPE::LEVEL2;
                    break;
                default:
                    next = GAME_STATE_TYPE::LEVEL1;
                    break;
                }
                SaveManager::GetInstance().toContinue = false;
            }
        }
        };

    collider->OnTriggerExit = [this](Collider* other, int sides) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
            this->interactionTextBox->isActive = false;
    };

    interactionTextBox->text = "[F] Enter";

}

void CheckpointTile::Init() {
    Tile::Init();
    collider->isTrigger = true;
    collider->OnTriggerEnter = [this](Collider* other, int sides) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player)
        {
            this->interactionTextBox->isActive = true;
            interactionTextBox->text = "Saved!";
            SaveManager::GetInstance().SaveAll();
           /* SaveManager::GetInstance().SavePlayerData(
            );
            MapManager::GetInstance().SaveMapState(current);
            EnemyManager::GetInstance().SaveEnemyStates();*/
        }
        };
    collider->OnTriggerExit = [this](Collider* other, int sides) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
        {
            this->interactionTextBox->isActive = false;
        }
        };
}

void CrateTile::Init()
{
    Tile::Init();
    collider->size.x = 0.875f;
    collider->size.y = 0.875f;

    interactionTextBox->text = "[F] Grab";

    collider->OnCollisionEnter = [this](Collider* other, int sides) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player && !pushState)
            interactionTextBox->isActive = true;

        Arrow* arrow = dynamic_cast<Arrow*>(other->owner);
        if (arrow && arrow->isActive)
        {
            int hitSides = collider->GetSidesForCollider(other);
            if (hitSides & COLLISION_SIDE::LEFT || hitSides & COLLISION_SIDE::RIGHT)
            {
                rb->velocity.x = (hitSides & COLLISION_SIDE::LEFT) ? 200.0f : -200.0f;
            }
            arrow->isActive = false;
            arrow->timer = 0.0f;
        }
        };

    collider->OnCollisionOver = [this](Collider* other, int sides) {
        Player* player = dynamic_cast<Player*>(other->owner);
        if (player)
        {
            if (AEInputCheckTriggered(AEVK_F)) {
                pushState = !pushState;
                if (pushState) {
                    grabbedPlayer = player;
                    grabbedPlayer->currentAction = PlayerAction::CRATEINTERACT;
                    grabbedSide = playerOnLeft ? COLLISION_SIDE::LEFT : COLLISION_SIDE::RIGHT;
                    interactionTextBox->isActive = false;
                }
                else {
                    grabbedPlayer->currentAction = PlayerAction::IDLE;
                    grabbedPlayer = nullptr;
                    grabbedSide = 0;
                    interactionTextBox->isActive = true;
                }
            }
        }
        };

    collider->OnCollisionExit = [this](Collider* other, int sides) {
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
            grabbedPlayer->currentAction = PlayerAction::IDLE;
            grabbedPlayer = nullptr;
            grabbedSide = 0;
        }
    }

    if (pushState && grabbedPlayer && grabbedPlayer->currentAction == PlayerAction::CRATEINTERACT)
    {
        // release grab when F is pressed
        if (AEInputCheckTriggered(AEVK_F))
        {
            pushState = false;
            grabbedPlayer->currentAction = PlayerAction::IDLE;
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
            if (dynamic_cast<LeverTile*>(tile)) continue;

            Collider* oCol = tile->collider;
            if (!oCol) continue;
            if (BoxToBoxCollision(
                collider->GetPos2D(), oCol->GetPos2D(),
                collider->GetScale(), oCol->GetScale()))
            {
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
