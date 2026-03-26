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
};

class ParticleSystem {
public:
    static void Init(u32 maxParticles, std::vector<Particle>& particlePool);
    static void CreateBloodEffect(f32 x, f32 y, std::vector<Particle>& particlePool); // Orange blood
    static void CreateArrowTrail(f32 x, f32 y, AEVec2 direction, std::vector<Particle>& particlePool);

    static void Update(f32 dt, std::vector<Particle>& particlePool);
    static void Draw(std::vector<Particle>& particlePool);
    static void Exit();

private:
    //static std::vector<Particle> particlePool;
    static AEGfxVertexList* pParticleMesh;
};

#endif
