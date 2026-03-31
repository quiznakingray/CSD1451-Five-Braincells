#ifndef PARTICLE_EFFECTS_H
#define PARTICLE_EFFECTS_H

#include "AEEngine.h"
#include <vector>


struct Particle {
    AEVec2   pos = { 0.0f, 0.0f }; // Default to origin
    AEVec2   vel = { 0.0f, 0.0f }; // No initial movement
    f32      lifespan = 0.0f; // Start dead
    f32      maxLifespan = 0.0f;
    u32      color = 0xFFFFFFFF; 
    bool     active = false; // Ensure particles start inactive
	f32 size = 1.0f; // Default size
};

class ParticleSystem {
public:
    void Init(u32 maxParticles);
    void CreateHitEffect(f32 x, f32 y); // Orange blood
    void CreateArrowTrail(f32 x, f32 y, AEVec2 direction);

    void Update(f32 dt);
    void Draw();
    void Exit();

private:
    std::vector<Particle> particlePool;
    AEGfxVertexList* pParticleMesh = nullptr;
};

#endif
