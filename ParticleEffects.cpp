//---------------------------------------------------------
// author:    Ang Zi Jun & Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#include "ParticleEffects.h"
#include "PlayerManager.h"
#include "CameraSystem.h"



void ParticleSystem::Init(u32 maxParticles, bool is_Circle) {
    particlePool.resize(maxParticles);
	isCircle = is_Circle;
    // Create a simple square mesh for particles
    AEGfxMeshStart();
    if (isCircle)
    {
        f32 slice = 40.0f;
        f32 theta = 360.0f / slice;
        for (int i = 1; i <= slice; i++)
        {
            f32 x1 = AECosDeg((i - 1) * theta);
            f32 y1 = AESinDeg((i - 1) * theta);
            f32 uCircle1 = (x1 + 1.0f) * 0.5f;
            f32 vCircle1 = 1.0f - ((y1 + 1.0f) * 0.5f);
            AEGfxVertexAdd(x1, y1, 0xFFFFFFFF, uCircle1, vCircle1);

            // Center vertex
            AEGfxVertexAdd(0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f);

            f32 x2 = AECosDeg(i * theta);
            f32 y2 = AESinDeg(i * theta);
            f32 uCircle2 = (x2 + 1.0f) * 0.5f;
            f32 vCircle2 = 1.0f - ((y2 + 1.0f) * 0.5f);
            AEGfxVertexAdd(x2, y2, 0xFFFFFFFF, uCircle2, vCircle2);
        }
        
    }
    else {

        AEGfxTriAdd(-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 1.0f,
            0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
        AEGfxTriAdd(0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
            0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
            -0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);
    }
    pParticleMesh = AEGfxMeshEnd();
}

void ParticleSystem::CreateHitEffect(f32 x, f32 y) {

    u32 colors[] = { 0xFFFFFF00, 0xFFFF8800, 0xFFFFFFFF, 0xFF88FFFF, 0xFFFF4400 };

    int count = 0;
    for (auto& p : particlePool)
    {
        if (!p.active)
        {
            p.active = true;
            p.pos = { x, y };

            // Spread evenly in burst pattern
            f32 angle = (360.0f / 8.0f) * count;
            f32 speed = 80.0f + AERandFloat() * 120.0f; // 80-200
            p.vel.x = AECosDeg(angle) * speed;
            p.vel.y = AESinDeg(angle) * speed;

            p.color = colors[rand() % 5];
            p.maxLifespan = 0.3f + AERandFloat() * 0.2f; // 0.3-0.5s
            p.lifespan = p.maxLifespan;
            p.size = 8.0f + AERandFloat() * 50.0f;
            count++;
            if (count >= 8) break;
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
            p.vel = { -direction.x * 0.05f, -direction.y * 0.05f };
			p.size = 10.f + AERandFloat() * 20.f; // Randomize size a bit
            p.maxLifespan = p.lifespan = 2.f; // Shorter life for trails
            p.color = 0xFFFFFFFF; // White/Grey trail
            break; // Only spawn 1 particle per frame for a smooth line
        }
    }
}

void ParticleSystem::CreateFireRain(f32 screenW, f32 screenH)
{
    AEVec2 cam;
    AEGfxGetCamPosition(&cam.x, &cam.y);

    // Spawn a few fire particles per frame across the top of the screen
    int spawned = 0;
    for (auto& p : particlePool)
    {
        if (!p.active && spawned < 30)
        {
            p.active = true;
            // Random X across screen width, start at top
            p.pos.x = cam.x + (-screenW * 0.5f) + AERandFloat() * screenW;
            p.pos.y = cam.y + screenH * 0.5f + 20.f;

            // Fall downward with slight horizontal drift
            p.vel.x = (AERandFloat() - 0.5f) * 40.f;
            p.vel.y = -(80.f + AERandFloat() * 60.f);
            p.rotation = AERandFloat() * 360.f;           
            p.rotationSpeed = (AERandFloat() - 0.5f) * 180.f;
            // Fire colors: red, orange, yellow
            u32 fireColors[] = { 0xFFFF2200, 0xFFFF6600, 0xFFFFAA00, 0xFFFFFF00 };
            p.color = fireColors[rand() % 4];

            p.maxLifespan = p.lifespan = 1.5f + AERandFloat() * 10.0f;
            p.size = 10.f + AERandFloat() * 20.f;
            spawned++;
        }
    }
}

void ParticleSystem::CreateCheckpointExplosion(f32 x, f32 y)
{
    u32 colors[] = { 0xFFFF0066, 0xFF00FFFF, 0xFFFF00FF, 0xFF00FF00,
                     0xFFFFFF00, 0xFFFF6600, 0xFF6600FF, 0xFFFFFFFF };
    int count = 0;
    for (auto& p : particlePool)
    {
        if (!p.active)
        {
            p.active = true;
            p.pos = { x, y };

            f32 angle = 30.f + AERandFloat() * 120.f;
            f32 speed = 200.f + AERandFloat() * 350.f; // fast upward burst
            p.vel.x = AECosDeg(angle) * speed;
            p.vel.y = AESinDeg(angle) * speed;

            p.color = colors[rand() % 8];
            p.maxLifespan = p.lifespan = 0.8f + AERandFloat() * 0.8f;
            p.size = 12.f + AERandFloat() * 25.f;
            p.rotation = AERandFloat() * 360.f;
            p.rotationSpeed = (AERandFloat() - 0.5f) * 720.f;
            count++;
            if (count >= 20) break;
        }
    }
}
void ParticleSystem::CreateEnemyStatic(f32 x, f32 y)
{
    // TV static / electric shock — white, cyan, grey flickering pixels
    u32 colors[] = { 0xFFFFFFFF, 0xFF00FFFF, 0xFFAAAAAA, 0xFF88FFFF, 0xFF44FFFF };
    int count = 0;
    for (auto& p : particlePool)
    {
        if (!p.active)
        {
            p.active = true;

            // Scatter randomly around enemy, not in a radial burst
            p.pos.x = x + (AERandFloat() - 0.5f) * 80.f;
            p.pos.y = y + (AERandFloat() - 0.5f) * 80.f;

            // Erratic movement — TV static feel
            p.vel.x = (AERandFloat() - 0.5f) * 150.f;
            p.vel.y = (AERandFloat() - 0.5f) * 150.f;

            p.color = colors[rand() % 5];
            p.maxLifespan = p.lifespan = 0.1f + AERandFloat() * 0.3f; // very short flicker
            p.size = 20.f + AERandFloat() * 40.f;

            count++;
            if (count >= 15) break;
        }
    }
}
void ParticleSystem::Update(f32 dt) {
    for (auto& p : particlePool) {
        if (p.active) {
            p.pos.x += p.vel.x * dt;
            p.pos.y += p.vel.y * dt;
            p.rotation += p.rotationSpeed * dt;
            p.lifespan -= dt;

            if (p.lifespan <= 0) p.active = false;
        }
    }
}

void ParticleSystem::Draw() {
    AEGfxSetRenderMode(AE_GFX_RM_COLOR);

    // Get the camera position so particles stay in the game world
    //AEVec2 camPos = CameraSystem::GetCameraPos();

    for (auto& p : particlePool) {
        if (p.active) {
            AEMtx33 scale, trans, rot, temp, res;
            f32 size = (p.lifespan / p.maxLifespan) * p.size;

            AEMtx33Scale(&scale, size, size);
            AEMtx33Rot(&rot, AEDegToRad(p.rotation));
            // SUBTRACT camPos here so particles move correctly with the level
            AEMtx33Trans(&trans, p.pos.x , p.pos.y );

            AEMtx33Concat(&temp, &rot, &scale);       // rotate then scale
            AEMtx33Concat(&res, &trans, &temp);

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