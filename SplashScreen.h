//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

class SplashScreen
{
public:
    static void Load();
    static void Init();
    static void Update();
    static void Render();
    static void Free();
    static void Unload();
};

#endif