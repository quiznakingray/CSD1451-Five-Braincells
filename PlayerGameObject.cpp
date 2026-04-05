#include "AEEngine.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include "PlayerManager.h"
#include "SaveManager.h"
#include "PlayerStats.h"
#include "ParticleEffects.h"
#include "EnemyGameObject.h"
#include "EnemyCombat.h"

#include <iostream>
#include <vector>

static std::array<TILE_ID, 4> noJumpTiles = { TILE_ID::SPIKEDOWN, TILE_ID::SPIKEUP, TILE_ID::SPIKELEFT, TILE_ID::SPIKERIGHT };

void Player::PlayerInput()
{
	f64 dt = AEFrameRateControllerGetFrameTime();
	AEVec2 moveDir{};

	bool isGrabbing = currentAction == PLAYER_ACTION::CRATEINTERACT;
	float accel = isGrabbing ? 200.0f : 300.0f;
	float jumpHeight = 350.0f;  // lower jump when grabbing

	//AEVec2Set(&velocity, 0.f, 0.f);
	//std::cout << "On GRASSCENTER: " << (onGround ? "--" : "___________________________ ") << std::endl;

	if (AEInputCheckTriggered(AEVK_SPACE) && rb->onCollider && !isGrabbing && canJump
		&& PlayerStats::GetInstance().jumpStamina != 0)
	{
		if (PlayerStats::GetInstance().ConsumeJumpStamina())
		{
			float jumpVelocity = sqrtf(2.0f * fabs(rb->gravity) * jumpHeight);
			rb->velocity.y = jumpVelocity;
		}
		else
		{
			std::cout << "[Jump] no stamina!\n";
		}
		//std::cout << "onCollider: " << rb->onCollider << std::endl;
	}

	static float walkTimer = 0.0f; // timer for walking sfx
	bool isWalking = false;

	if (AEInputCheckCurr(AEVK_A))
	{
		//moveDir.x -= 1.f;
		rb->velocity.x -= static_cast<f32>(accel * dt);
		isWalking = true;
	}
	else if (AEInputCheckCurr(AEVK_D))
	{
		//moveDir.x += 1.f;
		rb->velocity.x += static_cast<f32>(accel * dt);
		isWalking = true;
	}
	else
	{
		ApplyDeceleration();
	}

	const float walkInterval = 0.3f; // play every 0.3s while walking
	if (isWalking && rb->onCollider) // only when colliding with wall, floor, player, etc.
	{
		walkTimer -= static_cast<float>(dt);
		if (walkTimer <= 0.0f)
		{
			AudioManager::GetInstance().PlaySFX("walk");
			walkTimer = walkInterval;
		}
	}
}

void Player::PlayerAction()
{
	if (rb->velocity.y != 0) {
		currentAction = PLAYER_ACTION::JUMPING;
	}
	else {

		if (rb->velocity.x != 0 && currentAction != PLAYER_ACTION::CRATEINTERACT)
		{
			currentAction = PLAYER_ACTION::RUNNING;
		}
		else if (rb->velocity.x < 0.1f && currentAction != PLAYER_ACTION::CRATEINTERACT){
			currentAction = PLAYER_ACTION::IDLE;
		}
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
		anim = jumpAnim;
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
	float decel = isGrabbing ? 300.0f : 500.0f;

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

void Player::InitAbilityBar()
{
	float barW = 10.f;
	float barH = scale.y * 0.8f;
	float offsetX = scale.x / 2.f + 20.f; // float to the left

	// Background (dark red)
	abilityBarBG = new GameObject(barW, barH, pos.x - offsetX, pos.y, pos.z + 0.1f, 0, false);
	abilityBarBG->AddComponent(new Sprite())->meshColor = 0xFF440000;

	// Foreground (bright green)
	abilityBarFG = new GameObject(barW, barH, pos.x - offsetX, pos.y, pos.z + 0.2f, 0, false);
	abilityBarFG->AddComponent(new Sprite())->meshColor = 0xFF00FF00;

	abilityBarObjects.push_back(abilityBarBG);
	abilityBarObjects.push_back(abilityBarFG);
	InitGameObjects(abilityBarObjects);

}

void Player::UpdateAbilityBar(float currentValue, float max)
{
	if (!abilityBarBG || !abilityBarFG) return;

	float barH = scale.y * 0.8f;
	float offsetX = scale.x / 2.f + 20.f;

	float ratio = currentValue / max;
	ratio = ratio < 0.f ? 0.f : ratio > 1.f ? 1.f : ratio;

	// Follow enemy position
	abilityBarBG->pos.x = pos.x - offsetX;
	abilityBarBG->pos.y = pos.y;

	// Shrink 
	abilityBarFG->pos.x = pos.x - offsetX;
	abilityBarFG->pos.y = pos.y - (barH * (1.f - ratio)) / 2.f;
	abilityBarFG->scale.y = barH * ratio;

	
	abilityBarBG->isActive = (currentValue <= max);
	abilityBarFG->isActive = (currentValue <= max);

	// Color shifts green -> yellow -> red as health drops
	if (ratio > 0.5f)       abilityBarFG->GetComponent<Sprite>()->meshColor = 0xFF00FF00;
	else if (ratio > 0.25f) abilityBarFG->GetComponent<Sprite>()->meshColor = 0xFF00FFFF;
	else                    abilityBarFG->GetComponent<Sprite>()->meshColor = 0xFF0000FF;

	UpdateGameObjects(abilityBarObjects);
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
	if (jumpAnim) {
		delete jumpAnim;
		jumpAnim = nullptr;
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

	if (!jumpAnim)
	{
		Sprite* jump = new Sprite();
		jump->meshColor = 0xFF0000FF;
		jump->textureFileName = "Assets/SpriteSheets/testRed4x6.png";
		jump->spriteSheet = Sprite::SpriteSheet(6, 4);
		jump->spriteSheet.isSpriteSheet = true;

		jumpAnim = new Animation(jump);

	}

	animator = AddComponent(
		new Animator(idleAnim)
	);
	//showColliders = true;
	Collider* c = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.f, 0.f, 0.8f, 1.f)
	);
	//c->OnClick = [] {
	//	std::cout << "Clicking" << std::endl;
	//	};
	//c->OnMouseDown = [] {
	//	std::cout << "Mouse Down" << std::endl;
	//	};
	//c->OnMouseUp = [] {
	//	std::cout << "Mouse Up" << std::endl;
	//	};

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
				if ((sides & COLLISION_SIDE::BOTTOM))
					this->rb->onCollider = true;
				
				if (std::find(noJumpTiles.begin(), noJumpTiles.end(), tile->currID) != noJumpTiles.end()) {
					canJump = false;
					std::cout << "cannot jump\n";
				}
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

	c->OnCollisionExit = [this](Collider* other, int )
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				this->rb->onCollider = false;  
				if (std::find(noJumpTiles.begin(), noJumpTiles.end(), tile->currID) != noJumpTiles.end()) {
					canJump = true;
					//std::cout << "can jump\n";
				}
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
	hurtParticles.Init(5);
	InitAbilityBar();
	GameObject::Init();
}

void Player::Update(){
	float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());
	//PlayerStats::Get().RegenStamina(dt); // passive stamina regen

	PlayerAction();
	bool isGrabbing = currentAction == PLAYER_ACTION::CRATEINTERACT;
	float maxSpeed = isGrabbing ? 200.0f : 250.0f;
	if (rb->velocity.x > maxSpeed)
		rb->velocity.x = maxSpeed;

	if (rb->velocity.x < -maxSpeed)
		rb->velocity.x = -maxSpeed;
	animator->PlayAnimation(PlayerAnimation());

	runningAnim->sprite->size.x =
		fabs(runningAnim->sprite->size.x) * (rb->velocity.x < 0 ? -1 : 1);
	jumpAnim->sprite->size.x =
		fabs(runningAnim->sprite->size.x) * (rb->velocity.x < 0 ? -1 : 1);

	if (gotHurt)
	{
		hurtDuration -= dt;
		if (hurtDuration <= 0.f)
		{
			gotHurt = false;
			hurtDuration = 2.0f;
		}
	}
	hurtParticles.CreateHitEffect(pos.x, pos.y);
	hurtParticles.Update(dt);

	bool isOnGround = rb->onCollider;

	// Detect from air to ground
	if (!wasOnGround && isOnGround)
	{
		if (rb->velocity.y < -50.0f)
		{
			AudioManager::GetInstance().PlaySFX("land");
		}
	}

	// Store state for next frame
	wasOnGround = isOnGround;

	GameObject::Update();
	//std::cout << "Pos: " << pos.x << "   " << pos.y << "  Velocity: " << rb->velocity.x <<"  " << rb->velocity.y << std::endl;
}

void Player::Render()
{
	if (gotHurt)
		hurtParticles.Draw();
	GameObject::Render();
}

void Player::Free()
{
	GameObject::Free();
	hurtParticles.Exit();

	FreeGameObjects(abilityBarObjects);
	for (GameObject* go : abilityBarObjects)
		delete go;
	abilityBarObjects.clear();
}

void Player::TakeDamage(int amount)
{
	gotHurt = true;
	PlayerStats::GetInstance().ReducePlayerHealth(amount);

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

	Sprite* jump = new Sprite();
	jump->meshColor = 0xFF0000FF;
	jump->textureFileName = "Assets/SpriteSheets/Player_Melee_Jump.png";
	jump->spriteSheet = Sprite::SpriteSheet(1, 1);
	jump->spriteSheet.isSpriteSheet = true;

	jumpAnim = new Animation(jump);
	jumpAnim->loopAnimation = true;
	jumpAnim->animationFPS = 30.f;

	 //shield collider
	shieldCollider = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.6f, 0.0f, 0.3f, 0.8f)
		);
	shieldCollider->isTrigger = true;

	//shieldCollider->OnTriggerEnter = [this](Collider* other, int)
	//	{
			//if (!shieldActive) return;   // shield is down = do nothing

			//if (Arrow* arrow = dynamic_cast<Arrow*>(other->owner))
			//{
			//	if (arrow->isEnemyProjectile)
			//	{
			//		arrow->isActive = false;
			//		arrow->timer = 0.f;
			//		std::cout << "[Shield] blocked incoming projectile!\n";
			//	}
			//}
			// add enemy projectile types!!
	//};
	

	 //shielding
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

	 //shield animation 
	shieldBubbleAnimation = AddComponent(new Animator(shieldAmin));
	shieldBubbleAnimation->Init();
	
	Player::Init();
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
	//float dt = static_cast<float>(AEFrameRateControllerGetFrameTime());

	// shield input

	
	shieldCollider->isActive = shieldActive;
	shieldBubbleAnimation->isActive = shieldActive;
	//std::cout << "Shield Pos: " << pos.x << "   " << pos.y << "  Velocity: " << rb->velocity.x << "  " << rb->velocity.y << std::endl;
	Player::Update();
}

void MeleePlayer::Render()
{
	GameObject::Render();
	if (inShieldAction)
	{
		RenderGameObjects(abilityBarObjects);
	}
}

void MeleePlayer::PlayerInput()
{
	f64 dt = AEFrameRateControllerGetFrameTime();
	
	bool qHeld = AEInputCheckCurr(AEVK_Q);
	if (qHeld)
	{
		ApplyDeceleration();
		inShieldAction = true;
		if (!shieldDepleted)
		{
			shieldTimer += static_cast<float>(dt);
			//currentAction = PLAYER_ACTION::SHIELDING;
			float maxDuration = PlayerStats::GetInstance().GetMaxShieldDuration();
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
		UpdateAbilityBar(PlayerStats::GetInstance().GetMaxShieldDuration() - shieldTimer, PlayerStats::GetInstance().GetMaxShieldDuration());
		return;
	}
	else {

	
		shieldActive = false;

		//// reset stamina when Q released
		//if (!qHeld)
		//{
			shieldTimer = 0.0f;
			shieldDepleted = false;
		//}
		
		inShieldAction = false;
	}

	Player::PlayerInput();

}

void MeleePlayer::PlayerAction()
{
	// shield has prio
	if (inShieldAction)
	{
		//prevAction = currentAction; 
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

void MeleePlayer::TakeDamage(int amount)
{
	if(shieldActive)
	{
		std::cout << "[Shield] blocked incoming damage!\n";
		return;
	}
	Player::TakeDamage(amount);
}

RangePlayer::~RangePlayer()
{
	if (aimingAnim) {
		delete aimingAnim;
		aimingAnim = nullptr;
	}
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

	Sprite* jump = new Sprite();
	jump->meshColor = 0xFF0000FF;
	jump->textureFileName = "Assets/SpriteSheets/Player_Range_Jump.png";
	jump->spriteSheet = Sprite::SpriteSheet(1, 1);
	jump->spriteSheet.isSpriteSheet = true;

	jumpAnim = new Animation(jump);
	jumpAnim->loopAnimation = true;
	jumpAnim->animationFPS = 30.f;

	Sprite* aimingSprite = new Sprite();
	aimingSprite->meshColor = 0xFF0000FF;
	aimingSprite->textureFileName = "Assets/SpriteSheets/Player_Range_Aim.png";
	aimingSprite->spriteSheet = Sprite::SpriteSheet(1, 1);
	aimingSprite->spriteSheet.isSpriteSheet = true;

	aimingAnim = new Animation(aimingSprite);
	aimingAnim->loopAnimation = true;
	//aimingAnim->animationFPS = 30.f;

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

	// init particle pool
	//ParticleSystem::Init(5, particlePool);

	Player::Init();
}

void RangePlayer::Update()
{
	// cooldown timer
	double dt = AEFrameRateControllerGetFrameTime();
	arrowTimer += static_cast<float>(dt);
	canShoot = arrowTimer >= PlayerStats::GetInstance().GetAttackCooldown();
	s32 screenX, screenY;
	//f32 camPosX, camPosY;
	f32 worldPosX, worldPosY;
	AEInputGetCursorPosition(&screenX, &screenY);
	//AEGfxGetCamPosition(&camPosX, &camPosY);
	worldPosX = screenX + AEGfxGetWinMinX();
	worldPosY = -(screenY - AEGfxGetWinMaxY());
	//std::cout << worldPosX << "   " << worldPosY << std::endl;
	playerLinePos->pos.x = pos.x + (worldPosX < pos.x ? -1 : 1) * MapManager::tileSize / 2.f;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = worldPosX;
	aimLinePos->pos.y = worldPosY;
	aimingAnim->sprite->size.x =
		fabs(runningAnim->sprite->size.x) * (worldPosX < pos.x ? -1 : 1);

	//ParticleSystem::CreateBloodEffect(pos.x, pos.y + 100, particlePool);
	//ParticleSystem::Update(dt, particlePool);
	if (!canShoot)
		UpdateAbilityBar(arrowTimer, PlayerStats::GetInstance().GetAttackCooldown());
	Player::Update();
}

//void RangePlayer::Render()
//{
//	ParticleSystem::Draw(particlePool); // paticles
//	Player::Render(); // parent function
//}

void RangePlayer::PlayerInput()
{
	if (AEInputCheckTriggered(AEVK_Q))
		AudioManager::GetInstance().PlaySFX("aimBow");

	if (AEInputCheckCurr(AEVK_Q))
	{
		inAimingAnim = true;
		if (AEInputCheckTriggered(AEVK_LBUTTON))
		{
			// shoot if cooldown has passed
			//float cooldown = PlayerStats::GetInstance().GetAttackCooldown();

			if (canShoot)
			{
				// fire arrow
				AEVec2 dir;
				AEVec2Sub(&dir, &aimLinePos->pos, &playerLinePos->pos);

				// normalize so components are between -1 and 1
				AEVec2Normalize(&dir, &dir);

				//PlayerManager::rangePlayerArrow->ShootArrow(playerLinePos->pos, dir);
				ShootArrow();

				// reset timer
				arrowTimer = 0.0f;
			}
			else
			{
				//std::cout << "[Bow] still on cooldown!" << (cooldown - arrowTimer) << "s remaining\n";
			}
		}
		return;
	}
	else {
		inAimingAnim = false;
	}
	Player::PlayerInput();
}

void RangePlayer::Render()
{
	if (!canShoot)
		RenderGameObjects(abilityBarObjects);
	Player::Render();
}

void RangePlayer::PlayerAction()
{
	//if (rb->velocity.x != 0 && currentAction != PLAYER_ACTION::CRATEINTERACT)
	//{
	//	prevAction = currentAction;
	//	currentAction = rb->velocity.y != 0 ? PLAYER_ACTION::JUMPING : PLAYER_ACTION::RUNNING;
	//}
	if (inAimingAnim)
	{
		currentAction = PLAYER_ACTION::AIMING;
		return;
	}

	Player::PlayerAction();
}

Animation* RangePlayer::PlayerAnimation()
{
	if (!animator) return nullptr;
	Animation* anim = Player::PlayerAnimation();

	if (currentAction == PLAYER_ACTION::AIMING)
	{
		anim = aimingAnim;
	}
	return anim;
}

void RangePlayer::ShootArrow()
{
	for (Arrow* arrow : PlayerManager::GetInstance().arrowGameObjectPool)
	{
		if (!arrow->isActive)
		{
			AEVec2 dir;
			AEVec2Sub(&dir, &aimLinePos->pos, &playerLinePos->pos);
			// normalize so components are between -1 and 1
			AEVec2Normalize(&dir, &dir);
			arrow->ShootArrow(playerLinePos->pos, dir);
			return;
		}
	}

	// else if no available arrows in pool
	//std::cout << "[Bow] no arrows available in pool!\n";
}

void Arrow::Init()
{
	AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);
	Sprite* s = AddComponent(new Sprite());
	//s->meshColor = 0xFF0000FF;
	s->textureFileName = "Assets/SpriteSheets/arrow.png";

	Collider* c = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0.f, 0.f, 1.f, 0.5f)
	);
	c->OnTriggerEnter = [this](Collider* other, int )
		{
			//if (((sides & COLLISION_SIDE::LEFT) && rb->velocity.x < 0) ||
			//	((sides & COLLISION_SIDE::RIGHT) && rb->velocity.x > 0))
			//{
				if (Tile* tile = dynamic_cast<Tile*>(other->owner))
				{
					// crate will deactivate arrow on its own ontrigger
					if (dynamic_cast<CrateTile*>(other->owner) || tile->canArrowPass) return;

					isActive = false;
					timer = 0.0f;
					AudioManager::GetInstance().PlaySFX("arrowHit");
					return;
				}

				if (isEnemyProjectile)
				{
					if (Player* player = dynamic_cast<Player*>(other->owner))
					{
						if (other->isTrigger) return; // ignore if it's the player's shield collider
						player->TakeDamage(damage);
						isActive = false;
						AudioManager::GetInstance().PlaySFX("arrowHit");
						return;
					}
				}
				else {
					if (EnemyGameObject* enemy = dynamic_cast<EnemyGameObject*>(other->owner))
					{
						damage = PlayerStats::GetInstance().GetPlayerDamage();
						//std::cout << "Hit enemy for " << damage << " damage!\n";

						isActive = false;
						EnemyTakeDamage(*enemy, damage);
						AudioManager::GetInstance().PlaySFX("arrowHit");
						return;
					}
				}

			//}
		};
	c->isTrigger = true;
	//showColliders = true;

	rb = AddComponent(
		new RigidBody()
	);
	rb->type = RIGIDBODY_TYPE::KINEMATIC;
	rb->hasGravity = false;
	isActive = false;

	particlePool.Init(5);

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

		AEVec2 dir;
		AEVec2Normalize(&dir, &rb->velocity);

		// Offset behind arrow (negative direction)
		float tailOffsetX = -dir.x * scale.x / 2.f;
		float tailOffsetY = -dir.y * scale.y / 2.f;

		particlePool.CreateArrowTrail(
			pos.x + tailOffsetX,
			pos.y + tailOffsetY,
			rb->velocity);

		particlePool.Update(static_cast<f32>(dt));
	}
}

void Arrow::Render()
{
	if (isActive) particlePool.Draw();
	GameObject::Render();
}

void Arrow::Free()
{
	GameObject::Free();
	particlePool.Exit();
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
	s->size.x = fabs(s->size.x);
	AudioManager::GetInstance().PlaySFX("shootArrow");
}
