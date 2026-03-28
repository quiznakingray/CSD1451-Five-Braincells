#include "AEEngine.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include "PlayerManager.h"
#include "SaveManager.h"
#include "PlayerStats.h"

#include <iostream>
#include <vector>

void Player::PlayerInput()
{
	f64 dt = AEFrameRateControllerGetFrameTime();
	AEVec2 moveDir{};

	bool isGrabbing = currentAction == PLAYER_ACTION::CRATEINTERACT;
	float accel = isGrabbing ? 200.0f : 300.0f;
	float maxSpeed = isGrabbing ? 200.0f : 300.0f;
	float jumpHeight = 300.0f;  // lower jump when grabbing

	//AEVec2Set(&velocity, 0.f, 0.f);
	//std::cout << "On GRASSCENTER: " << (onGround ? "--" : "___________________________ ") << std::endl;
	if (AEInputCheckTriggered(AEVK_SPACE) && rb->onCollider && !isGrabbing 
		&& PlayerStats::Get().jumpStamina != 0)
	{
		if (PlayerStats::Get().ConsumeJumpStamina())
		{
			float jumpVelocity = sqrtf(2.0f * fabs(rb->gravity) * 350.0f);
			rb->velocity.y = jumpVelocity;
		}
		else
		{
			std::cout << "[Jump] no stamina!\n";
		}
		//std::cout << "onCollider: " << rb->onCollider << std::endl;
	}

	if (AEInputCheckCurr(AEVK_A))
	{
		//moveDir.x -= 1.f;
		rb->velocity.x -= static_cast<f32>(accel * dt);
	}
	else if (AEInputCheckCurr(AEVK_D))
	{
		//moveDir.x += 1.f;
		rb->velocity.x += static_cast<f32>(accel * dt);
	}
	else
	{
		ApplyDeceleration();

	}

	if (rb->velocity.x > maxSpeed)
		rb->velocity.x = maxSpeed;

	if (rb->velocity.x < -maxSpeed)
		rb->velocity.x = -maxSpeed;

}

void Player::PlayerAction()
{
	if (rb->velocity.x != 0 && currentAction != PLAYER_ACTION::CRATEINTERACT)
	{
		prevAction = currentAction;
		currentAction = rb->velocity.y != 0 ? PLAYER_ACTION::JUMPING : PLAYER_ACTION::RUNNING;
	}
	else if (rb->velocity.x < 0.1f && currentAction != PLAYER_ACTION::CRATEINTERACT){
		prevAction = currentAction;
		currentAction = PLAYER_ACTION::IDLE;
	}
}

Animation* Player::PlayerAnimation()
{
	if (!animator) return nullptr;
	Animation* anim = idleAnim;

	if (currentAction == PLAYER_ACTION::RUNNING)
	{
		anim = runningAnim;
	}

	if (currentAction == PLAYER_ACTION::JUMPING)
	{

	}

	return anim;
}

void Player::ResetPlayer()
{
	currentAction = PLAYER_ACTION::IDLE;
}

void Player::ApplyDeceleration()
{
	if (!rb) return;
	f64 dt = AEFrameRateControllerGetFrameTime();
	bool isGrabbing = currentAction == PLAYER_ACTION::CRATEINTERACT;
	float decel = isGrabbing ? 300.0f : 400.0f;
	float maxSpeed = isGrabbing ? 200.0f : 300.0f;

	if (rb->velocity.x > 0)
	{
		rb->velocity.x -= static_cast<f32>(decel * dt);
		if (rb->velocity.x < 0) rb->velocity.x = 0;
	}
	else if (rb->velocity.x < 0)
	{
		rb->velocity.x += static_cast<f32>(decel * dt);
		if (rb->velocity.x > 0) rb->velocity.x = 0;
	}

	//if (rb->velocity.x > maxSpeed)  rb->velocity.x = maxSpeed;
	//if (rb->velocity.x < -maxSpeed) rb->velocity.x = -maxSpeed;
}

Player::~Player()
{
	if (idleAnim) {
		delete idleAnim;
		idleAnim = nullptr;
	}
	if (runningAnim) {
		delete runningAnim;
		runningAnim = nullptr;
	}
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
		Sprite* s = new Sprite();
		s->meshColor = 0xFF0000FF;
		s->textureFileName = "Assets/SpriteSheets/test2x12.png";
		s->spriteSheet = Sprite::SpriteSheet(12, 2);
		s->spriteSheet.isSpriteSheet = true;

		idleAnim = new Animation(s);

	}

	if (!runningAnim)
	{
		Sprite* run = new Sprite();
		run->meshColor = 0xFF0000FF;
		run->textureFileName = "Assets/SpriteSheets/testRed4x6.png";
		run->spriteSheet = Sprite::SpriteSheet(6, 4);
		run->spriteSheet.isSpriteSheet = true;

		runningAnim = new Animation(run);

	}

	animator = AddComponent(
		new Animator(idleAnim)
	);

	Collider* c = AddComponent(
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
	c->OnMouseOver = [] {
		std::cout << "Mouse Over" << std::endl;
		};	
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
			if (CrateTile* crate = dynamic_cast<CrateTile*>(other->owner))
			{
				if (sides & COLLISION_SIDE::BOTTOM)
					this->rb->onCollider = true;
			}
		};

	c->OnCollisionOver = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				if (sides & COLLISION_SIDE::BOTTOM)
					this->rb->onCollider = true;
			}
			if (CrateTile* crate = dynamic_cast<CrateTile*>(other->owner))
			{
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


	//showColliders = true;
	speed = static_cast<f32>(200.0);
	//AEVec2Set(&velocity, 0.f, 0.f);

	GameObject::Init();
}

void Player::Update(){
	//float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
	//PlayerStats::Get().RegenStamina(dt); // passive stamina regen

	//PlayerInput();

	animator->PlayAnimation(PlayerAnimation());

	runningAnim->sprite->size.x =
		fabs(runningAnim->sprite->size.x) * (rb->velocity.x < 0 ? -1 : 1);
	GameObject::Update();
	//std::cout << "Pos: " << pos.x << "   " << pos.y << "  Velocity: " << rb->velocity.x <<"  " << rb->velocity.y << std::endl;
}

void Player::TakeDamage(int amount)
{
	PlayerStats::Get().health -= amount;

	if (PlayerStats::Get().health <= 0)
	{
		// inc death counter
		PlayerStats::Get().deathCount++;

		// respawn: reset health and jump stamina
		PlayerStats::Get().health = PlayerStats::Get().maxHealth;
		PlayerStats::Get().jumpStamina =PlayerStats::Get().maxJumpStamina;

		std::cout << "[Player] Total deaths: " << PlayerStats::Get().deathCount << "\n";
	}
}

void Player::IncrementKills()
{
	PlayerStats::Get().killCount++;
	std::cout << "[Player] Total kills: " << PlayerStats::Get().killCount << "\n";
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

	// shield collider
	shieldCollider = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.6f, 0.0f, 0.3f, 0.8f)
		);
	shieldCollider->isTrigger = true;

	shieldCollider->OnTriggerEnter = [this](Collider* other, int sides)
		{
			if (!shieldActive) return;   // shield is down = do nothing

			if (Arrow* arrow = dynamic_cast<Arrow*>(other->owner))
			{
				if (arrow->isEnemyProjectile)
				{
					arrow->isActive = false;
					arrow->timer = 0.f;
					std::cout << "[Shield] blocked incoming projectile!\n";
				}
			}
			// add enemy projectile types!!
	};
	
	Player::Init();

	// shielding
	Sprite* shielding = new Sprite();
	shielding->meshColor = 0xFF0000FF;
	shielding->textureFileName = "Assets/SpriteSheets/Player_Melee_Shielding.png";
	shielding->spriteSheet = Sprite::SpriteSheet(2, 7);
	shielding->spriteSheet.isSpriteSheet = true;

	shieldingAnim = new Animation(shielding);
	shieldingAnim->loopAnimation = true;
	shieldingAnim->animationFPS = 15.f;

	// shield animator
	Sprite *shieldSprite = new Sprite(1.5f, 1.5f);
	shieldSprite->textureFileName = "Assets/SpriteSheets/Player_Melee_Bubble_Shield.png";
	shieldSprite->spriteSheet = Sprite::SpriteSheet(2, 7);
	shieldSprite->spriteSheet.isSpriteSheet = true;

	shieldAmin = new Animation(shieldSprite);
	shieldAmin->loopAnimation = true;
	shieldAmin->animationFPS = 15.f;

	// shield animation 
	shieldBubbleAnimation = AddComponent(new Animator(shieldAmin));
	shieldBubbleAnimation->Init();;
}
MeleePlayer::~MeleePlayer()
{
	if (shieldingAnim) {
		delete shieldingAnim;
		shieldingAnim = nullptr;
	}
	if (shieldAmin) {
		delete shieldAmin;
		shieldAmin = nullptr;
	}
}
void MeleePlayer::Update()
{
	float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

	// shield input
	if (PlayerManager::GetInstance().currentPlayer == this)
	{
		bool qHeld = AEInputCheckCurr(AEVK_Q);

		if (qHeld && !shieldDepleted)
		{
			shieldTimer += dt;

			float maxDuration = PlayerStats::Get().GetMaxShieldDuration();
			if (shieldTimer >= maxDuration)
			{
				// stamina exhausted
				shieldTimer = maxDuration;
				shieldDepleted = true;
				shieldActive = false;
				std::cout << "[Shield] stamina ran out! release Q to recover stamina.\n";
			}
			else
			{
				shieldActive = true;
			}
		}
		else
		{
			shieldActive = false;

			// reset stamina when Q released
			if (!qHeld)
			{
				shieldTimer = 0.0f;
				shieldDepleted = false;
			}
		}

	}
	shieldCollider->isActive = shieldActive;
	shieldBubbleAnimation->isActive = shieldActive;
	
	Player::Update();
}

void MeleePlayer::PlayerAction()
{
	// shield has prio
	if (AEInputCheckCurr(AEVK_Q))
	{
		prevAction = currentAction; 
		currentAction = PLAYER_ACTION::SHIELDING;
		return;
	}
	Player::PlayerAction();

}

Animation* MeleePlayer::PlayerAnimation()
{
	if (!animator) return nullptr;
	Animation* anim = Player::PlayerAnimation();

	if (currentAction == PLAYER_ACTION::SHIELDING)
	{
		anim = shieldingAnim;
	}
	return anim;
}

RangePlayer::~RangePlayer()
{
	line = nullptr;
	playerLinePos = nullptr;  // non-owning, Sprite::Free() handles delete
	aimLinePos = nullptr;
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
	playerLinePos->pos.x = pos.x + MapManager::tileSize / 2.f;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = pos.x + 10.f;
	aimLinePos->pos.y = pos.y;
	line->linePoints.push_back(playerLinePos);
	line->linePoints.push_back(aimLinePos);

	Player::Init();
}

void RangePlayer::Update()
{
	// cooldown timer
	double dt = AEFrameRateControllerGetFrameTime();
	arrowTimer += static_cast<float>(dt);

	s32 screenX, screenY;
	//f32 camPosX, camPosY;
	f32 LEVEL1PosX, LEVEL1PosY;
	AEInputGetCursorPosition(&screenX, &screenY);
	//AEGfxGetCamPosition(&camPosX, &camPosY);
	LEVEL1PosX = screenX + AEGfxGetWinMinX();
	LEVEL1PosY = -(screenY - AEGfxGetWinMaxY());
	//std::cout << LEVEL1PosX << "   " << LEVEL1PosY << std::endl;
	playerLinePos->pos.x = pos.x + (LEVEL1PosX < pos.x ? -1 : 1) * MapManager::tileSize / 2.f;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = LEVEL1PosX;
	aimLinePos->pos.y = LEVEL1PosY;
	Player::Update();
}

void RangePlayer::PlayerAction()
{
	Player::PlayerAction();
	if (rb->velocity.x != 0 && currentAction != PLAYER_ACTION::CRATEINTERACT)
	{
		prevAction = currentAction;
		currentAction = rb->velocity.y != 0 ? PLAYER_ACTION::JUMPING : PLAYER_ACTION::RUNNING;
	}
	if (AEInputCheckCurr(AEVK_Q))
	{
		currentAction = PLAYER_ACTION::AIMING;
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			// shoot if cooldown has passed
			float cooldown = PlayerStats::Get().GetAttackCooldown();

			if (arrowTimer >= cooldown)
			{
				// fire arrow
				AEVec2 dir;
				AEVec2Sub(&dir, &aimLinePos->pos, &playerLinePos->pos);

				// normalize so components are between -1 and 1
				AEVec2Normalize(&dir, &dir);

				PlayerManager::rangePlayerArrow->ShootArrow(playerLinePos->pos, dir);

				// reset timer
				arrowTimer = 0.0f;
			}
			else
			{
				std::cout << "[Bow] still on cooldown!" << (cooldown - arrowTimer) << "s remaining\n";
			}
		}
	}
	else if (rb->velocity.x < 0.1f && currentAction != PLAYER_ACTION::CRATEINTERACT) {
		prevAction = currentAction;
		currentAction = PLAYER_ACTION::IDLE;
	}
}

void Arrow::Init()
{
	AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);
	Sprite* s = AddComponent(new Sprite());
	s->meshColor = 0xFF0000FF;
	s->textureFileName = "Assets/SpriteSheets/arrow.png";

	Collider* c = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.f, 0.f, 1.f, 0.5f)
	);
	c->OnTriggerEnter = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				// crate will deactivate arrow on its own ontrigger
				if (dynamic_cast<CrateTile*>(other->owner)) return;
				if (((sides & COLLISION_SIDE::LEFT) && rb->velocity.x < 0) ||
					((sides & COLLISION_SIDE::RIGHT) && rb->velocity.x > 0))
				{
					isActive = false;
					timer = 0.0f;

				}

			}
		};
	c->isTrigger = true;
	//showColliders = true;

	rb = AddComponent(
		new RigidBody()
	);
	rb->type = RIGIDBODY_TYPE::KINEMATIC;
	rb->hasGravity = false;
	isActive = false;
	isEnemyProjectile = false;
	damage = 1;
	GameObject::Init();
}

void Arrow::Update()
{
	GameObject::Update();
	if (isActive)
	{
		double dt = AEFrameRateControllerGetFrameTime();
		timer += static_cast<f32>(dt);
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
