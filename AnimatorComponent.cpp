#include "AnimatorComponent.h"

Animation::~Animation()
{
	Free();
	if (sprite) {
		delete sprite;
		sprite = nullptr;
	}
}
void Animation::Init() {
	if (!sprite) return;
	sprite->Init();
	sprite->spriteSheet.currentFrame = 0; // can change
	sprite->UpdateFrame();
}

void Animation::Free() {
	if (!sprite) return;
	sprite->Free();
}
Animator::~Animator()
{
	if (currentAnimation) {
		currentAnimation = nullptr;
	}
	if (nextAnimation) {
		nextAnimation = nullptr;
	}
}
void Animator::PlayAnimation(Animation* a)
{
	// if it already playing
	if (currentAnimation == a || nextAnimation == a) return;

	//set next animation
	nextAnimation = a;
	nextAnimation->sprite->owner = owner;
}

void Animator::Init()
{
	if (currentAnimation) {

		currentAnimation->sprite->owner = owner;
		currentAnimation->Init();
	}
}

void Animator::Update()
{
	if (!currentAnimation) return;

	// do animation 
	Sprite* s = currentAnimation->sprite;
	if (!s->spriteSheet.isSpriteSheet) return;
	animationTimer += (f32)AEFrameRateControllerGetFrameTime();
	if (animationTimer >= 1.0f / currentAnimation->animationFPS)
	{
		animationTimer = 0.0f;
		if (s->spriteSheet.currentFrame >= s->spriteSheet.maxFrames - 1 && !currentAnimation->loopAnimation) return;

		s->spriteSheet.currentFrame = ++s->spriteSheet.currentFrame % s->spriteSheet.maxFrames;
		s->UpdateFrame();
	}
	
	if (nextAnimation && currentAnimation != nextAnimation)
	{
		// free current animation
		//currentAnimation->Free();
		currentAnimation = nextAnimation;

		// initialize next animation
		currentAnimation->Init();
		animationTimer = 0.0f;
		nextAnimation = nullptr;

	}

}

void Animator::Render()
{
	if (!currentAnimation)return;

	currentAnimation->sprite->Render();
}


void Animator::Free()
{
	if (currentAnimation) currentAnimation->Free();
	if (nextAnimation) nextAnimation->Free();

}
