#include "MapManager.h"
#include "SpriteManager.h"
#include <array>
#include <algorithm>

#pragma region MapFuncs

rapidcsv::Document map;
std::vector<std::vector<std::vector<Tile>>> arrMapInfo;
AEGfxVertexList* mesh;

void MapManager::InitMap(std::string fileName, unsigned int currLevel)
{
    map = rapidcsv::Document(fileName);
    // Read a row from the CSV file
    size_t x = (map.GetRow<std::string>(0)).size();
    size_t y = (map.GetColumn<std::string>(0)).size();
    arrMapInfo.resize(MAX_LEVELS, std::vector<std::vector<Tile>>(y, std::vector<Tile>(x)));

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
            std::cout << arrMapInfo[currLevel][uiRow][uiCol].currID << ' ';

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
            if (arrMapInfo[currLevel][uiRow][uiCol].currID) {
                if (arrMapInfo[currLevel][uiRow][uiCol].currID != arrMapInfo[currLevel][uiRow][uiCol].bgID && 
                     arrMapInfo[currLevel][uiRow][uiCol].isBGActive)
                {
                    RenderSprite(arrMapInfo[currLevel][uiRow][uiCol].bgSprite, mesh);
                }
                if (arrMapInfo[currLevel][uiRow][uiCol].isCurrActive) {
                    RenderSprite(arrMapInfo[currLevel][uiRow][uiCol].currSprite, mesh);
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

Tile MapManager::InitTile(int mapIndex, std::string cell, size_t col, size_t row)
{
    // saves first int as current currID of tile
    Tile newTile;
    int currID = stoi(cell);
    cell.erase(0, 4);
    int bgID = currID;
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
                bgID = stoi(cell);
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

    newTile.currID = currID;
    newTile.bgID = bgID;
    newTile.currTag = currTag;
    newTile.ogTag = currTag;
    newTile.isBGActive = bgActive;
    newTile.isCurrActive = currActive;
    // sets size, position, row, col of tile
    AEVec2Set(&newTile.currSprite.scale, tileSize, tileSize);
    AEVec2 size = newTile.currSprite.scale;
    f32 x = -((f32)AEGfxGetWindowWidth() * 0.5f) + ((size.x) * (col + 1));
    f32 y = ((f32)AEGfxGetWindowHeight() * 0.5f) - ((size.y) * (row + 1));
    AEVec2Set(&newTile.currSprite.pos, x, y);
    CheckTileToInit(&newTile);

    // if bg tile sprite is present
    if (bgID != currID) 
    {
        AEVec2Set(&newTile.bgSprite.scale, tileSize, tileSize);
        AEVec2 size = newTile.bgSprite.scale;
        f32 x = -((f32)AEGfxGetWindowWidth() * 0.5f) + ((size.x) * (col + 1));
        f32 y = ((f32)AEGfxGetWindowHeight() * 0.5f) - ((size.y) * (row + 1));
        AEVec2Set(&newTile.bgSprite.pos, x, y);
        newTile.bgSprite.texture = SetTileTexture(bgID);
    }
    newTile.row = row;
    newTile.col = col;
    newTile.currSprite.texture = SetTileTexture(currID);
    return newTile;
}

AEGfxTexture* MapManager::SetTileTexture(unsigned int currID)
{
	AEGfxTexture* tTex;
	switch (currID) 
    {
    case EMPTY:
        tTex = nullptr;
        break;
    case GROUND:
        // grass image
        tTex = AEGfxTextureLoad("Assets/Environment/ground.png");
        break;
    case WALL:
        // gray stone image
        tTex = AEGfxTextureLoad("Assets/Environment/wall.png");
        break;
    case GOAL:
        tTex = AEGfxTextureLoad("Assets/Environment/doorClose.png");
        break;
    case LASERRED:
        tTex = AEGfxTextureLoad("Assets/Environment/laserRedVertical.png");
        break;
    case LASERGREEN:
        tTex = AEGfxTextureLoad("Assets/Environment/laserGreenVertical.png");
        break;
    case LEVERREDON:
        tTex = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOn.png");
        break;
    case LEVERREDOFF:
        tTex = AEGfxTextureLoad("Assets/Environment/laserRedSwitchOff.png");
        break;
    case LEVERGREENON:
        tTex = AEGfxTextureLoad("Assets/Environment/laserGreenSwitchOn.png");
        break;
    case LEVERGREENOFF:
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
        case SPIKEDOWN:
        {
            tile->currSprite.rotation = 0;
            break;
        }
        case SPIKEUP:
        {
            tile->currSprite.rotation = PI;
            break;
        }
        case SPIKELEFT:
        {
            tile->currSprite.rotation = 270 * (PI / 180);
            break;
        }
        case SPIKERIGHT:
        {
            tile->currSprite.rotation = PI / 2;
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
    return &arrMapInfo[mapCurrLevel][col][row];
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
            if (arrMapInfo[mapCurrLevel][uiCol][uiRow].currTag == tag)
            {
                taggedTiles.push_back(&arrMapInfo[mapCurrLevel][uiCol][uiRow]);
                i++;
            }

        }
    }
    return taggedTiles;
    
}

std::vector<Tile*> MapManager::GetTaggedTiles(int tag, int id)
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
            if (arrMapInfo[mapCurrLevel][uiCol][uiRow].currTag == tag && 
                arrMapInfo[mapCurrLevel][uiCol][uiRow].currID == id)
            {
                taggedTiles.push_back(&arrMapInfo[mapCurrLevel][uiCol][uiRow]);
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
