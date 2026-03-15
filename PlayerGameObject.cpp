#include "AEEngine.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include "PlayerManager.h"

#include <iostream>
#include <vector>

void Player::PlayerInput()
{
	f32 dt = AEFrameRateControllerGetFrameTime();
	AEVec2 moveDir{};

	bool isGrabbing = currentAction == PlayerAction::CRATEINTERACT;
	float accel = isGrabbing ? 150.0f : 300.0f;
	float decel = isGrabbing ? 300.0f : 400.0f;
	float maxSpeed = isGrabbing ? 150.0f : 300.0f;
	float jumpHeight = 300.0f;  // lower jump when grabbing

	//AEVec2Set(&velocity, 0.f, 0.f);
	//std::cout << "On ground: " << (onGround ? "--" : "___________________________ ") << std::endl;
	if (AEInputCheckTriggered(AEVK_SPACE) && rb->onCollider && !isGrabbing)
	{
		//moveDir.y = 500.f;
		//onGround = false;
		float jumpVelocity = sqrtf(2.0f * fabs(rb->gravity) * 300.0f);
		rb->velocity.y = jumpVelocity;
	}
	//if (AEInputCheckCurr(AEVK_S))
	//{
	//	moveDir.y -= 1.f;
	//}
	if (AEInputCheckCurr(AEVK_A))
	{
		//moveDir.x -= 1.f;
		rb->velocity.x -= accel * dt;
	}
	else if (AEInputCheckCurr(AEVK_D))
	{
		//moveDir.x += 1.f;
		rb->velocity.x += accel * dt;
	}
	else
	{
		if (rb->velocity.x > 0)
		{
			rb->velocity.x -= decel * dt;
			if (rb->velocity.x < 0)
				rb->velocity.x = 0;
		}
		else if (rb->velocity.x < 0)
		{
			rb->velocity.x += decel * dt;
			if (rb->velocity.x > 0)
				rb->velocity.x = 0;
		}

	}

	if (rb->velocity.x > maxSpeed)
		rb->velocity.x = maxSpeed;

	if (rb->velocity.x < -maxSpeed)
		rb->velocity.x = -maxSpeed;
	//float length = sqrt(moveDir.x * moveDir.x);
	//if (length > 0) {
	//	moveDir.x /= length;
	//}

	// Set velocity
	//rb->velocity.x = moveDir.x * speed;
	//rb->velocity.y += moveDir.y ;

	//if (AEInputCheckCurr(AEVK_M))
	//{
	//	animator->PlayAnimation(runningAnim);
	//}
	//if (AEInputCheckCurr(AEVK_N))
	//{
	//	animator->PlayAnimation(idleAnim);
	//}
}

void Player::PlayerAction()
{
	//}
	if (rb->velocity.x != 0 && currentAction != PlayerAction::CRATEINTERACT)
	{
		prevAction = currentAction;
		currentAction = rb->velocity.y != 0 ? PlayerAction::JUMPING : PlayerAction::RUNNING;
	}
	else if (rb->velocity.x < 0.1f && currentAction != PlayerAction::CRATEINTERACT){
		prevAction = currentAction;
		currentAction = PlayerAction::IDLE;
	}
}

void Player::PlayerAnimation()
{
	if (!animator) return;
	Animation* anim = nullptr;
	switch (currentAction)
	{
	case PlayerAction::IDLE:
		anim = idleAnim;
		break;
	case PlayerAction::RUNNING:
		anim = runningAnim;
		break;
	default:
		anim = idleAnim;
		break;
	}

	animator->PlayAnimation(anim);
}

void Player::Init()
{

	//set pos
	//AEVec2Set(&pos,MapManager::GetPlayerSpawnPos().x, MapManager::GetPlayerSpawnPos().y + 200.f);
	pos.z = 1.f;

	// set state
	objectState = STATE::IDLE;

	//set scale
	AEVec2Set(&scale, MapManager::tileSize - 10, MapManager::tileSize - 10);

	// set components

	//set animation
	if (!idleAnim)
	{
		Sprite * s = new Sprite();
		s->meshColor = 0xFF0000FF;
		s->textureFileName = "Assets/SpriteSheets/test2x12.png";
		s->spriteSheet = Sprite::SpriteSheet(12, 2);
		s->spriteSheet.isSpriteSheet = true;

		idleAnim = new Animation(s);

	}

	if (!runningAnim)
	{
		Sprite * run = new Sprite();
		run->meshColor = 0xFF0000FF;
		run->textureFileName = "Assets/SpriteSheets/testRed4x6.png";
		run->spriteSheet = Sprite::SpriteSheet(6, 4);
		run->spriteSheet.isSpriteSheet = true;

		runningAnim = new Animation(run);

	}

	animator = AddComponent(
		new Animator(idleAnim)
	);

	Collider * c = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.f, 0.f, 1.f, 1.f)
	);
	c->OnClick = [] {
		std::cout << "Clicking" << std::endl;
		};
	c->OnMouseDown = [] {
		std::cout << "Mouse Down" << std::endl;
		};	
	c->OnMouseUp = [] {
		std::cout << "Mouse Up" << std::endl;
		};

	//c->OnMouseEnter = [] {	
	//	std::cout << "Mouse Enter" << std::endl;
	//	};
	//c->OnMouseOver = [] {
	//	std::cout << "Mouse Over" << std::endl;
	//	};	
	//c->OnMouseExit = [] {
	//	std::cout << "Mouse Exit" << std::endl;
	//	};

	c->OnCollisionEnter = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				if (sides & COLLISION_SIDE::BOTTOM)
					this->rb->onCollider = true;

			}
		};

	c->OnCollisionOver = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				// Keep onCollider true while still standing on something
				if (sides & COLLISION_SIDE::BOTTOM)
					this->rb->onCollider = true;
			}
		};

	c->OnCollisionExit = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				this->rb->onCollider = false;
			}
		};

	rb = AddComponent(
		new RigidBody()
	);
	rb->type = RIGIDBODY_TYPE::DYNAMIC;
	rb->mass = 10.f;

	//AEGfxSetCamPosition(pos.x, pos.y);


	//s32 screenX, screenY;
	//f32 camPosX, camPosY;
	//f32 worldPosX, worldPosY;
	//AEInputGetCursorPosition(&screenX, &screenY);
	//AEGfxGetCamPosition(&camPosX, &camPosY);
	//worldPosX = camPosX - screenX / 2.f;
	//worldPosY = camPosY - screenY / 2.f;

	showColliders = true;
	speed = static_cast<f32>(200.0);
	//AEVec2Set(&velocity, 0.f, 0.f);

	GameObject::Init();
}

void Player::Update(){
	PlayerInput();
	PlayerAction();
	PlayerAnimation();

	runningAnim->sprite->size.x =
		fabs(runningAnim->sprite->size.x) * (rb->velocity.x < 0 ? -1 : 1);


	GameObject::Update();
	//std::cout << "Pos: " << pos.x << "   " << pos.y << "  Velocity: " << rb->velocity.x <<"  " << rb->velocity.y << std::endl;
}

void MeleePlayer::Init()
{

	Sprite* s = new Sprite();
	s->meshColor = 0xFF0000FF;
	s->textureFileName = "Assets/SpriteSheets/Player_Melee_Idle.png";
	s->spriteSheet = Sprite::SpriteSheet(2, 7);
	s->spriteSheet.isSpriteSheet = true;

	idleAnim = new Animation(s);
	idleAnim->loopAnimation = true;
	idleAnim->animationFPS = 10.f;
	
	Sprite* run = new Sprite();
	run->meshColor = 0xFF0000FF;
	run->textureFileName = "Assets/SpriteSheets/Player_Melee_Run.png";
	run->spriteSheet = Sprite::SpriteSheet(2, 7);
	run->spriteSheet.isSpriteSheet = true;

	runningAnim = new Animation(run);
	runningAnim->loopAnimation = true;
	runningAnim->animationFPS = 30.f;
	Player::Init();
}

void RangePlayer::Init()
{

	Sprite* s = new Sprite();
	s->meshColor = 0xFF0000FF;
	s->textureFileName = "Assets/SpriteSheets/Player_Range_Idle.png";
	s->spriteSheet = Sprite::SpriteSheet(2, 7);
	s->spriteSheet.isSpriteSheet = true;

	idleAnim = new Animation(s);
	idleAnim->loopAnimation = true;
	idleAnim->animationFPS = 10.f;
	
	Sprite* run = new Sprite();
	run->meshColor = 0xFF0000FF;
	run->textureFileName = "Assets/SpriteSheets/Player_Range_Run.png";
	run->spriteSheet = Sprite::SpriteSheet(2, 7);
	run->spriteSheet.isSpriteSheet = true;

	runningAnim = new Animation(run);
	runningAnim->loopAnimation = true;
	runningAnim->animationFPS = 30.f;

	line = AddComponent(new Sprite()
	);
	line->spriteShape = SPRITE_SHAPE::SHAPE_LINE;
	line->thickness = 5.f;
	line->meshColor = 0xFFFF0000;
	playerLinePos = new Sprite::LinePoint;
	aimLinePos = new Sprite::LinePoint;
	playerLinePos->pos.x = pos.x + MapManager::tileSize /2.f;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = pos.x + 10.f;
	aimLinePos->pos.y = pos.y;
	line->linePoints.push_back(playerLinePos);
	line->linePoints.push_back(aimLinePos);

	Player::Init();
}

void RangePlayer::Update()
{
	s32 screenX, screenY;
	//f32 camPosX, camPosY;
	f32 worldPosX, worldPosY;
	AEInputGetCursorPosition(&screenX, &screenY);
	//AEGfxGetCamPosition(&camPosX, &camPosY);
	worldPosX = screenX + AEGfxGetWinMinX();
	worldPosY = -(screenY - AEGfxGetWinMaxY());
	std::cout << worldPosX << "   " << worldPosY << std::endl;
	playerLinePos->pos.x = pos.x + (worldPosX < pos.x ? -1 : 1) * MapManager::tileSize / 2.f;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = worldPosX;
	aimLinePos->pos.y = worldPosY;
	Player::Update();
}

void RangePlayer::PlayerAction()
{
	if (rb->velocity.x != 0)
	{
		currentAction = rb->velocity.y != 0 ? PlayerAction::JUMPING : PlayerAction::RUNNING;
	}
	else if (AEInputCheckCurr(AEVK_LBUTTON))
	{
		currentAction = PlayerAction::AIMING;
		if (AEInputCheckTriggered(AEVK_RBUTTON))
		{
			// fire arrow
			AEVec2 dir;
			AEVec2Sub(&dir, &aimLinePos->pos, &playerLinePos->pos);

			// normalize so components are between -1 and 1
			AEVec2Normalize(&dir, &dir);

			PlayerManager::rangePlayerArrow->ShootArrow(playerLinePos->pos, dir);
		}
	}
	else {
		currentAction = PlayerAction::IDLE;
	}
}

void Arrow::Init()
{
	AEVec2Set(&scale, MapManager::tileSize , MapManager::tileSize );
	Sprite* s = AddComponent( new Sprite());
	s->meshColor = 0xFF0000FF;
	s->textureFileName = "Assets/SpriteSheets/arrow.png";

	Collider* c = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.f, 0.f, 1.f, 0.5f)
	);
	c->OnTriggerEnter = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				if (((sides & COLLISION_SIDE::LEFT) && rb->velocity.x < 0) || 
					((sides & COLLISION_SIDE::RIGHT) && rb->velocity.x > 0))
				{
					isActive = false;
					timer = 0.0f;

				}

			}
		};
	c->isTrigger = true;
	showColliders = true;

	rb = AddComponent(
		new RigidBody()
	);
	rb->type = RIGIDBODY_TYPE::DYNAMIC;
	rb->hasGravity = false;
	isActive = false;
	GameObject::Init();
}

void Arrow::Update()
{
	GameObject::Update();
	if (isActive)
	{
		double dt = AEFrameRateControllerGetFrameTime();
		timer += dt;
		if (timer >= lifetime)
		{
			isActive = false;
			timer = 0.0f;
		}
	}
}

void Arrow::ShootArrow(AEVec2 startPos, AEVec2 dir)
{
	if (isActive) return;
	pos.x = startPos.x;
	pos.y = startPos.y;
	isActive = true;

	// set velocity
	AEVec2Scale(&rb->velocity, &dir, speed);

	// rotate arrow to match direction
	rotation = atan2f(dir.y, dir.x);

	// flip sprite if moving left
	Sprite* s = GetComponent<Sprite>();
	if (rb->velocity.x < 0)
		s->size.x = -fabs(s->size.x);
	else
		s->size.x = fabs(s->size.x);
}
