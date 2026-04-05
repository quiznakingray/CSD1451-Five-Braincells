//---------------------------------------------------------
// author:    Ang Zi Jun & Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef PARTICLE_EFFECTS_H
#define PARTICLE_EFFECTS_H

#include "AEEngine.h"
#include <vector>


struct Particle {
    AEVec2   pos = { 0.0f, 0.0f }; // Default to origin
    AEVec2   vel = { 0.0f, 0.0f }; // No initial movement
    f32 rotation = 0.f;
    f32 rotationSpeed = 0.f;
    f32      lifespan = 0.0f; // Start dead
    f32      maxLifespan = 0.0f;
    u32      color = 0xFFFFFFFF; 
    bool     active = false; // Ensure particles start inactive
	f32 size = 1.0f; // Default size
};

class ParticleSystem {
public:
	bool isCircle = false; 
    void Init(u32 maxParticles, bool is_Circle = false);
    void CreateHitEffect(f32 x, f32 y); // Orange blood
    void CreateArrowTrail(f32 x, f32 y, AEVec2 direction);
    void CreateFireRain(f32 screenW, f32 screenH);
    void CreateCheckpointExplosion(f32 x, f32 y);
    void CreateEnemyStatic(f32 x, f32 y);
    void Update(f32 dt);
    void Draw();
    void Exit();

private:
    std::vector<Particle> particlePool;
    AEGfxVertexList* pParticleMesh = nullptr;
};

#endif
