
//---------------------------------------------------------
// author:    Kon Xin Yi
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "CooldownTimer.h"

#pragma region Timer
void CooldownTimer::Start(f32 d)
{
    duration = d;
    elapsed = 0.0;
    active = true;
}

void CooldownTimer::Stop()
{
    active = false;
}

bool CooldownTimer::Update(f32 dt)
{
    if (!active)
        return false;

    elapsed += dt;

    if (elapsed >= duration)
    {
        active = false;
        return true;   // finished this frame
    }

    return false;
}

f32 CooldownTimer::GetProgress() const
{
    if (duration <= 0.0)
        return 1.0;

    return (elapsed / duration);
}

bool CooldownTimer::IsActive() const
{
    return active;
}
#pragma endregion