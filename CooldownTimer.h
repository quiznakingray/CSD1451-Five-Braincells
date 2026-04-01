#pragma once
struct CooldownTimer
{
    double duration = 0.0;
    double elapsed = 0.0;
    bool active = false;

    void Start(double d);

    void Stop();

    bool Update(double dt);

    double GetProgress() const;

    bool IsActive() const;
};


