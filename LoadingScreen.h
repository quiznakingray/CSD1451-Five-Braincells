//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H
#include "GameStateManager.h"

class LoadingScreen
{
public:
    static GAME_STATE_TYPE targetState; // where to go after loading

    static void Load();
    static void Init();
    static void Update();
    static void Render();
    static void Free();
    static void Unload();
};
#endif