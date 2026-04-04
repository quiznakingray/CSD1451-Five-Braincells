#pragma once

#include "AEEngine.h"

struct AEVec3 : AEVec2 {
	f32 z;
    AEVec3(f32 x_ = 0.f, f32 y_ = 0.f, f32 z_ = 0.f) {
        x = x_;
        y = y_;
        z = z_;
    }
};
