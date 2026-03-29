#include "ParticleEffects.h"
#include "PlayerManager.h"
#include "CameraSystem.h"

std::vector<Particle> ParticleSystem::particlePool;
AEGfxVertexList* ParticleSystem::pParticleMesh = nullptr;

void ParticleSystem::Init(u32 maxParticles) {
    particlePool.resize(maxParticles);

    // Create a simple square mesh for particles
    AEGfxMeshStart();
    AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
        0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    AEGfxTriAdd(0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
        0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
        -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    pParticleMesh = AEGfxMeshEnd();
}

void ParticleSystem::CreateBloodEffect(f32 x, f32 y) {
    int count = 0;
    for (auto& p : particlePool) {
        if (!p.active) {
            p.active = true;
            p.pos = { x, y };

            // Random direction and speed for "splatter"
            f32 angle = AERandFloat() * 2.0f * PI;
            f32 speed = AERandFloat() * 100.0f + 50.0f;
            p.vel = { AECos(angle) * speed, AESin(angle) * speed };

            p.maxLifespan = p.lifespan = 0.5f; // Fast disappear
            p.color = 0xFFFFA500; // Orange color (AABBGGRR or hex)

            count++;
            if (count > 10) break; // Spawn 10 particles per hit
        }
    }
}

void ParticleSystem::CreateArrowTrail(f32 x, f32 y, AEVec2 direction) {
    for (auto& p : particlePool) {
        if (!p.active) {
            p.active = true;
            p.pos = { x, y };

            // Trail should move slightly opposite to the arrow's direction
            // We scale the direction down so the trail "lingers"
            p.vel = { -direction.x * 0.1f, -direction.y * 0.1f };

            p.maxLifespan = p.lifespan = 0.3f; // Shorter life for trails
            p.color = 0xFFFFFFFF; // White/Grey trail
            break; // Only spawn 1 particle per frame for a smooth line
        }
    }
}

void ParticleSystem::Update(f32 dt) {
    for (auto& p : particlePool) {
        if (p.active) {
            p.pos.x += p.vel.x * dt;
            p.pos.y += p.vel.y * dt;
            p.lifespan -= dt;

            if (p.lifespan <= 0) p.active = false;
        }
    }
}

void ParticleSystem::Draw() {
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);

    // Get the camera position so particles stay in the game world
    AEVec2 camPos = CameraSystem::GetCameraPos();

    for (auto& p : particlePool) {
        if (p.active) {
            AEMtx33 scale, trans, res;
            f32 size = (p.lifespan / p.maxLifespan) * 8.0f;

            AEMtx33Scale(&scale, size, size);

            // SUBTRACT camPos here so particles move correctly with the level
            AEMtx33Trans(&trans, p.pos.x - camPos.x, p.pos.y - camPos.y);

            AEMtx33Concat(&res, &trans, &scale);

            AEGfxSetTransform(res.m);

            // Use the particle's stored color (Orange for blood, White for arrows)
            // Extract RGB from hex for AEGfxSetColorToMultiply
            f32 r = ((p.color >> 16) & 0xFF) / 255.0f;
            f32 g = ((p.color >> 8) & 0xFF) / 255.0f;
            f32 b = (p.color & 0xFF) / 255.0f;
            f32 alpha = p.lifespan / p.maxLifespan;

            AEGfxSetColorToMultiply(r, g, b, alpha);
            AEGfxMeshDraw(pParticleMesh, AE_GFX_MDM_TRIANGLES);
        }
    }
}

void ParticleSystem::Exit() {
    if (pParticleMesh) AEGfxMeshFree(pParticleMesh);
}