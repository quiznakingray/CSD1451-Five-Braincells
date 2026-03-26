#pragma once
#include "AEEngine.h"
struct CooldownTimer
{
    f32 duration = 0.0;
    f32 elapsed = 0.0;
    bool active = false;

    void Start(f32 d);

    void Stop();

    bool Update(f32 dt);

    f32 GetProgress() const;

    bool IsActive() const;
};


