#pragma once
#include "AEEngine.h"

bool CheckBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size);

bool IsCursorOverRect(f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y);