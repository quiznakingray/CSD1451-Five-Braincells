//---------------------------------------------------------
// author:    Kon Xin Yi & Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef PARKOUT_LEVEL
#define PARKOUT_LEVEL

#include "GameStateManager.h"
struct ParkourLevel {
	static void Load() ;
	static void Init() ;
	static void Update() ;
	static void Render() ;
	static void Free() ;
	static void Unload() ;
	
};
#endif