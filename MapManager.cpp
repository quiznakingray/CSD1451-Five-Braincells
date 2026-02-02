#include "MapManager.h"
#include "SpriteManager.h"
#include "PlayerGameObject.h"
#include <array>
#include <algorithm>
#include <iostream>

#pragma region MapFuncs

rapidcsv::Document map;
std::vector<std::vector<std::vector<Tile*>>> arrMapInfo{};
AEGfxVertexList* mesh;

int MapManager::mapCurrLevel = 0;

void MapManager::InitMap(std::string fileName, unsigned int currLevel)
{
    map = rapidcsv::Document(fileName);
    // Read a row from the CSV file
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    arrMapInfo.resize(MAX_LEVELS, std::vector<std::vector<Tile *>>(y, std::vector<Tile *>(x)));

    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < x; ++uiCol)
        {

            arrMapInfo[currLevel][uiRow][uiCol] = InitTile(currLevel, row[uiCol], uiCol, uiRow);
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
    mapCurrLevel = currLevel;
}

void MapManager::PrintMap(unsigned int currLevel) {
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < x; ++uiCol)
        {
            std::cout << static_cast<int>(arrMapInfo[currLevel][uiRow][uiCol]->currID) << ' ';

        }
        std::cout << '\n';
    }
}
void MapManager::LoopMap(void* (mapfunc)())
{    
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < x; ++uiCol)
        {
            mapfunc();
        }
        std::cout << '\n';
    }
}
void MapManager::DrawMapSprite(int currLevel)
{
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < x; uiCol++)
        {
            Tile * currTile = arrMapInfo[currLevel][uiRow][uiCol];
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

// change this function to turn on/off tile collision
void MapManager::DrawMapCollision(int currLevel)
{
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < x; uiCol++)
        {
            //if (arrMapInfo[currLevel][uiRow][uiCol].currID) {
            //    if (arrMapInfo[currLevel][uiRow][uiCol].currID != arrMapInfo[currLevel][uiRow][uiCol].bgID &&
            //        arrMapInfo[currLevel][uiRow][uiCol].isBGActive)
            //    {
            //        RenderSprite(arrMapInfo[currLevel][uiRow][uiCol].bgSprite, mesh);
            //    }
            //    if (arrMapInfo[currLevel][uiRow][uiCol].isCurrActive) {
            //        RenderSprite(arrMapInfo[currLevel][uiRow][uiCol].currSprite, mesh);
            //    }
            //}
        }
    }
}

void MapManager::FreeMap()
{
    AEGfxMeshFree(mesh);
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

Tile* MapManager::InitTile(int mapIndex, std::string cell, size_t col, size_t row)
{
    // saves first int as current currID of tile
    TILE_ID currID = static_cast<TILE_ID>(stoi(cell));
    cell.erase(0, 4);
    TILE_ID bgID = currID;
    int currTag = 0;
    bool bgActive = true;
    bool currActive = true;

    int delimitCount = 0;
    while (!cell.empty()) {
        if (delimitCount == 0) {
            if (cell.length() == 1) {
                currTag = stoi(cell);
                cell.erase(0, cell.find(delimiter) + 1);
            }
            else {
                bgID = static_cast<TILE_ID>(stoi(cell));
                cell.erase(0, cell.find(delimiter) + 1);
            }
        }
        if (delimitCount == 1) {
            std::string third = cell.substr(0, cell.find(delimiter));
            if (third.length() == 1) {
                currTag = stoi(cell);
                cell.erase(0, cell.find(delimiter) + 1);
            }
            else {
                cell.erase(0, cell.find(delimiter) + 1);
                currActive = stoi(third);
            }
        }
        else if (delimitCount == 2) {
            std::string fourth = cell.substr(0, cell.find(delimiter));
            cell.erase(0, cell.find(delimiter) + 1);
            currActive = stoi(fourth);
        }
        else if (delimitCount == 3) {
            std::string fifth = cell.substr(0, cell.find(delimiter));
            cell.erase(0, cell.length());
            bgActive = stoi(fifth);
        }
        delimitCount++;
    }

    // do checking here
    Tile* newTile = currID != TILE_ID::SPIKEDOWN ?
        new Tile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize) :
        new SpikeTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);

    if (currID == TILE_ID::SPIKEDOWN)
    {
        Tile* t = newTile;
    }
    //newTile->currID = currID;
    //newTile->bgID = bgID;
    //newTile->currTag = currTag;
    //newTile->ogTag = currTag;
    //newTile->isBGActive = bgActive;
    //newTile->isCurrActive = currActive;
    // sets size, position, row, col of tile
    //AEVec2Set(&newTile->scale, tileSize, tileSize);
    //AEVec2 size = newTile->scale;
    //f32 x = -((f32)AEGfxGetWindowWidth() * 0.5f) + ((size.x) * (col + 1));
    //f32 y = ((f32)AEGfxGetWindowHeight() * 0.5f) - ((size.y) * (row + 1));
    //AEVec2Set(&newTile->pos, x, y);
    CheckTileToInit(newTile);

    //newTile->row = row;
    //newTile->col = col;
    if (currID == TILE_ID::PLAYER) return newTile;
    // if bg tile sprite is present
    if (bgID != currID) 
    {
        //AEVec2Set(&newTile->bgSprite.scale, tileSize, tileSize);
        //AEVec2 size = newTile->bgSprite.scale;
        //f32 x = -((f32)AEGfxGetWindowWidth() * 0.5f) + ((size.x) * (col + 1));
        //f32 y = ((f32)AEGfxGetWindowHeight() * 0.5f) - ((size.y) * (row + 1));
        //AEVec2Set(&newTile->bgSprite.pos, x, y);
        newTile->bgSprite = newTile->AddComponent(
            new Sprite()
        );
        newTile->bgSprite->texture = SetTileTexture(bgID);

    }


    newTile->currSprite->texture = SetTileTexture(currID); // can remove this after making structs for all kinds of tiles
    newTile->SetColliders();


    //newTile->showColliders = true;

    return newTile;
}

AEGfxTexture* MapManager::SetTileTexture(TILE_ID currID)
{
	AEGfxTexture* tTex;
	switch (currID) 
    {
    case TILE_ID::EMPTY:
        tTex = nullptr;
        break;
    case TILE_ID::GROUND:
        // grass image
        tTex = AEGfxTextureLoad("Assets/Environment/ground.png");
        break;
    case TILE_ID::WALL:
        // gray stone image
        tTex = AEGfxTextureLoad("Assets/Environment/wall.png");
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
        switch (tile->currID){
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
Tile* MapManager::GetTile(unsigned int col, unsigned int row)
{
    return arrMapInfo[mapCurrLevel][col][row];
}
std::vector<Tile*> MapManager::GetTaggedTiles(int tag)
{
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> taggedTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < x; uiCol++)
        {
            if (arrMapInfo[mapCurrLevel][uiCol][uiRow]->currTag == tag)
            {
                taggedTiles.push_back(arrMapInfo[mapCurrLevel][uiCol][uiRow]);
                i++;
            }

        }
    }
    return taggedTiles;
    
}

std::vector<Tile*> MapManager::GetTaggedTiles(int tag, TILE_ID id)
{
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    int i = 0;
    std::vector<Tile*> taggedTiles;
    //taggedTiles.resize(size, 0);
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        for (size_t uiCol = 0; uiCol < x; uiCol++)
        {
            if (arrMapInfo[mapCurrLevel][uiCol][uiRow]->currTag == tag && 
                arrMapInfo[mapCurrLevel][uiCol][uiRow]->currID == id)
            {
                taggedTiles.push_back(arrMapInfo[mapCurrLevel][uiCol][uiRow]);
                i++;
            }

        }
    }
    return taggedTiles;
}

#pragma endregion

#pragma region LaserFuncs
std::vector<Tile*> MapManager::GetTilesNearPos(AEVec2 pos, AEVec2 scale)
{


    std::vector<Tile*> nearbyTiles;

    for (std::vector<Tile*> row : arrMapInfo[mapCurrLevel])
    {
        // Load a particular CSV value into the arrMapInfo
        for (Tile* tile : row)
        {
            //Tile* currTile = arrMapInfo[mapCurrLevel][uiCol][uiRow];
            if (BoxToBoxCollision(pos, tile->pos, scale,  tile->scale) && tile->currID != TILE_ID::PLAYER)
            {
                nearbyTiles.push_back(tile);
            }
        }
    }
    return nearbyTiles;
}
void MapManager::SetLaserActive(Tile tile, bool active)
{
    std::vector<Tile*> lasers = GetTaggedTiles(tile.currTag, tile.currID);
    for (Tile* laser : lasers) {
        laser->isCurrActive = active;
    }
}
AEVec2 MapManager::GetPlayerSpawnPos()
{
    AEVec2 pos;
    AEVec2Set(&pos, 0, 0);

    //size_t x = (map.GetRow<std::string>(0)).size();
    //size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (std::vector<Tile*> row : arrMapInfo[mapCurrLevel])
    {
        // Load a particular CSV value into the arrMapInfo
        for (Tile* tile : row)
        {
            //Tile *currTile = arrMapInfo[mapCurrLevel][uiRow][uiCol];
            if (tile->currID == TILE_ID::PLAYER) {
                AEVec2Set(&pos, tile->pos.x, tile->pos.y);
            }
        }
    }
    return pos;
}
#pragma endregion


void Tile::Update()
{
    GameObject::Update();
}

void  MapManager::AddTilesToGameObjectVector(std::vector<GameObject*>& gos)
{
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (size_t uiRow = 0; uiRow < y; uiRow++)
    {
        // Load a particular CSV value into the arrMapInfo
        for (size_t uiCol = 0; uiCol < x; uiCol++)
        {
            Tile* currTile = arrMapInfo[mapCurrLevel][uiRow][uiCol];

            if(currTile->currID != TILE_ID::EMPTY) AddGameObjectToVector(currTile, gos);
        }
    }
}
