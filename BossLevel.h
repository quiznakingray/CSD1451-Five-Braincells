#ifndef BOSS_LEVEL
#define BOSS_LEVEL

#include "GameStateManager.h"
struct BossLevel {
	static void Load() ;
	static void Init() ;
	static void Update() ;
	static void Render() ;
	static void Free() ;
	static void Unload() ;
};
#endif