#include "MapManager.h"
#include "SpriteManager.h"

#pragma region MapFuncs

rapidcsv::Document map;
Tile arrMapInfo[MAX_LEVELS][MAX_YAXIS][MAX_XAXIS];
AEGfxVertexList* mesh;

void InitMap(std::string fileName, unsigned int currLevel)
{
    map = rapidcsv::Document(fileName);
    // Read a row from the CSV file
    int x = (map.GetRow<std::string>(0)).size();
    int y = (map.GetColumn<std::string>(0)).size();
    // Read a row from the CSV file
    std::vector<std::string> row = map.GetRow<std::string>(0);

    // Read the rows and columns of CSV data into arrMapInfo
    for (unsigned int uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (unsigned int uiCol = 0; uiCol < x; ++uiCol)
        {
            try {
                arrMapInfo[currLevel][uiRow][uiCol].currID = (int)stoi(row[uiCol]);
                if (arrMapInfo[currLevel][uiRow][uiCol].currID) {
                    InitTile(currLevel, arrMapInfo[currLevel][uiRow][uiCol].currID, uiCol, uiRow);
                }
            }
            catch (...) {
                arrMapInfo[currLevel][uiRow][uiCol].currID = 0;
            }
        }
    }
}

void PrintMap(unsigned int currLevel) {
    int x = (map.GetRow<std::string>(0)).size();
    int y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (unsigned int uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (unsigned int uiCol = 0; uiCol < x; ++uiCol)
        {
            std::cout << arrMapInfo[currLevel][uiRow][uiCol].currID << ' ';

        }
        std::cout << '\n';
    }
}
void LoopMap(void* (mapfunc)())
{    
    int x = (map.GetRow<std::string>(0)).size();
    int y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (unsigned int uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (unsigned int uiCol = 0; uiCol < x; ++uiCol)
        {
            mapfunc();
        }
        std::cout << '\n';
    }
}
void DrawMap(int currLevel)
{
    int x = (map.GetRow<std::string>(0)).size();
    int y = (map.GetColumn<std::string>(0)).size();
    // Read the rows and columns of CSV data into arrMapInfo
    for (unsigned int uiRow = 0; uiRow < y; uiRow++)
    {
        // Read a row from the CSV file
        std::vector<std::string> row = map.GetRow<std::string>(uiRow);

        // Load a particular CSV value into the arrMapInfo
        for (unsigned int uiCol = 0; uiCol < x; uiCol++) 
        {
            if (arrMapInfo[currLevel][uiRow][uiCol].currID) {
                DrawTile(arrMapInfo[currLevel][uiRow][uiCol].shape, arrMapInfo[currLevel][uiRow][uiCol].transform);
            }
        }
    }
}
void FreeMap()
{
    AEGfxMeshFree(mesh);
}
#pragma endregion

#pragma region TileFuncs
void DrawTile(Shape shape, AEMtx33 transform)
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
    AEGfxTextureSet(shape.tex, 0, 0);

        // Tell Alpha Engine to use the matrix in 'transform' to apply onto all
        // the vertices of the mesh that we are about to choose to draw in the next line.
    AEGfxSetTransform(transform.m);

        // Tell Alpha Engine to draw the mesh with the above settings.
    AEGfxMeshDraw(shape.mesh, AE_GFX_MDM_TRIANGLES);
}

void InitTile(int mapIndex, int currID, unsigned int col, unsigned int row)
{
    // saves first int as current currID of tile
    arrMapInfo[mapIndex][row][col].currID = currID;
    // sets size, position, row, col of tile
    AEVec2Set(&(arrMapInfo[mapIndex][row][col].shape.size), tileSize, tileSize);
    AEVec2 size = arrMapInfo[mapIndex][row][col].shape.size;

    f32 x = -((f32)AEGfxGetWindowWidth() * 0.5f) + ((size.x) * (col + 1));

    f32 y = -((f32)AEGfxGetWindowHeight() * 0.5f) + ((size.y) * (row + 1));
    AEVec2Set(&(arrMapInfo[mapIndex][row][col].shape.pos), x,y);
    arrMapInfo[mapIndex][row][col].row = row;
    arrMapInfo[mapIndex][row][col].col = col;
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
    arrMapInfo[mapIndex][row][col].shape.mesh = AEGfxMeshEnd();
    arrMapInfo[mapIndex][row][col].shape.tex = SetTileTexture(currID);
    TransformShape(arrMapInfo[mapIndex][row][col].shape.mesh, arrMapInfo[mapIndex][row][col].transform, arrMapInfo[mapIndex][row][col].shape);
}

AEGfxTexture* SetTileTexture(unsigned int currID)
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
        tTex = AEGfxTextureLoad("Assets/Environment/goal.png");
        break;
    case CRATE:
        tTex = AEGfxTextureLoad("Assets/Environment/crate.png");
        break;
    default:
        tTex = AEGfxTextureLoad("Assets/PlanetTexture.png");
        break;
	}
	return tTex;
}
#pragma endregion