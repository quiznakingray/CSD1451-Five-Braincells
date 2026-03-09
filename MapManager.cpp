#include "MapManager.h"
#include "SpriteManager.h"
#include "PlayerGameObject.h"
#include <array>
#include <algorithm>
#include <iostream>

#pragma region MapFuncs

rapidcsv::Document map;
std::vector<std::vector<Tile*>> arrMapInfo{};
AEGfxVertexList* mesh;

unsigned int MapManager::mapCurrLevel = 0;
unsigned int MapManager::rowCount = 0;
unsigned int MapManager::colCount = 0;

void MapManager::InitMap(std::string fileName, unsigned int currLevel)
{
    map = rapidcsv::Document(fileName);
    //mapCurrLevel = currLevel;
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

            arrMapInfo[uiRow][uiCol] = InitTile(mapCurrLevel, row[uiCol], uiCol, uiRow);
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

void MapManager::ChangeMap(unsigned int currLevel)
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
            if (tile && static_cast<int>(tile->currID) > -1)
            {
                tile->Free();   // frees all components (sprite, collider, text, etc.)
                delete tile;
            }
        }
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
            newTile = new LeverTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;

        case TILE_ID::GROUND:
            newTile = new GroundTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;
        case TILE_ID::WALL:
            newTile = new WallTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;

        case TILE_ID::LASERRED:
        case TILE_ID::LASERGREEN:
            newTile = new LaserTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;
        case TILE_ID::CLOUD:
            newTile = new CloudTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;
        case TILE_ID::CRATE:
            newTile = new CrateTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;
        case TILE_ID::BUTTONBLUEUNPRESSED:
            newTile = new ButtonTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
            break;
        case TILE_ID::GATE:
            newTile = new GateTile(currID, bgID, currTag, bgActive, currActive, row, col, tileSize);
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
    case TILE_ID::BUTTONBLUEUNPRESSED:
        tTex = AEGfxTextureLoad("Assets/Environment/buttonBlueUnpressed.png");
        break;
    case TILE_ID::GATE:
        tTex = AEGfxTextureLoad("Assets/Environment/gate.png");
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

void  MapManager::AddTilesToGameObjectVector(std::vector<GameObject*>& gos)
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

void CrateTile::Init()
{
    Tile::Init();
    collider->OnCollisionEnter = [this](Collider* other, int sides) {
    };

    collider->OnCollisionOver = [this](Collider* other, int sides) {

        };
    collider->OnCollisionExit = [this](Collider* other, int sides) {
        if (Player* player = dynamic_cast<Player*>(other->owner))
        {
            playerTouching = false;
            playerOnLeft = false;
            playerOnRight = false;
        }
        };
}

void CrateTile::Update() {
    Tile::Update();
    double dt = AEFrameRateControllerGetFrameTime();

   
    // Check crate against nearby tiles
    std::vector<Tile*> nearbyTiles = MapManager::GetTilesNearPos(pos, scale);
    std::vector<Collider*> colliders = GetComponents<Collider>();

for (Collider* pCol : colliders)
{
    float impulse = pushForce * dt;
    RigidBody* playerRb{};
    for (const auto& info : pCol->collisionInfos)
    {
        Collider* other = info.other;
        if (!other || !other->owner) continue;

        if (dynamic_cast<Player*>(other->owner))
        {
            playerTouching = true;
            playerRb = (*other->owner).GetComponent<RigidBody>();
            if (info.sides & COLLISION_SIDE::LEFT)
            playerOnLeft = true;

            if (info.sides & COLLISION_SIDE::RIGHT)
                playerOnRight = true;
        }
    }
    
    if (playerTouching) {

        if (playerOnLeft && AEInputCheckCurr(AEVK_D))
        {
            PhysicsManager::ApplyImpulse(rb, impulse);
        }


        if (playerOnRight && AEInputCheckCurr(AEVK_A))
        {
            PhysicsManager::ApplyImpulse(rb, -impulse);
        }
        std::cout << rb->velocity.x << '\n';
    }

    PhysicsManager::UpdateRigidBody(rb, static_cast<f32>(dt));


    if (!playerTouching)
    {
        if (rb->velocity.x > 0)
        {
            rb->velocity.x -= friction * dt;
            if (rb->velocity.x < 0)
                rb->velocity.x = 0;
            std::cout << rb->velocity.x << '\n';
        }
        else if (rb->velocity.x < 0)
        {
            rb->velocity.x += friction * dt;
            if (rb->velocity.x > 0)
                rb->velocity.x = 0;
            std::cout << rb->velocity.x << '\n';
        }
    }
    // Check nearby tiles for new collisions
    for (Tile* tile : nearbyTiles)
    {
        if (!tile->collider || !tile->collider->canCollide) continue;
        if (tile == this) continue;  // don't collide with self

        Collider* oCol = tile->collider;
        if (BoxToBoxCollision(
            pCol->GetPos2D(), oCol->GetPos2D(),
            pCol->GetScale(), oCol->GetScale()))
        {
            int sidesForCrate = GetAllCollisionSides(
                pCol->GetPos2D(), oCol->GetPos2D(),
                pCol->GetScale(), oCol->GetScale()
            );
            int sidesForTile = FlipCollisionSides(sidesForCrate);

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

    // Then resolve already-overlapping colliders (player, enemies, etc.)
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