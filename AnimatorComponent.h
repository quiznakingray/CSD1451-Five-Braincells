//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------

#ifndef ANIMATOR_COMPONENT
#define ANIMATOR_COMPONENT
#include "AEEngine.h"
#include "ComponentBase.h"
#include "SpriteManager.h"

struct Animation {
	Sprite* sprite = nullptr;
	f32 animationFPS = 1.f;
	bool loopAnimation = false;

	Animation(Sprite* s) : sprite(s){
	}
	~Animation();
	void Init();
	void Free();
};

// Animator component to handle sprite animations
struct Animator : ComponentBase{
	Animation* currentAnimation = nullptr;
	Animation* nextAnimation = nullptr; // next animation to transition to after current finishes (if not looping)

	f32 animationTimer = 0.0f;

	Animator(Animation* curr = nullptr) : currentAnimation(curr){
	}
	~Animator();
	void PlayAnimation(Animation* a); 
	

	void Init() override;
	void Update() override;
	void Render() override;
	void Free() override;
};
#endif // !ANIMATOR_COMPONENT
