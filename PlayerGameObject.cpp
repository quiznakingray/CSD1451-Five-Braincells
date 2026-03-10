#include "AEEngine.h"
#include "PlayerGameObject.h"
#include "MapManager.h"

#include <iostream>
#include <vector>

void Player::PlayerInput()
{
	f32 dt = AEFrameRateControllerGetFrameTime();
	AEVec2 moveDir{};
	//AEVec2Set(&velocity, 0.f, 0.f);
	//std::cout << "On ground: " << (onGround ? "--" : "___________________________ ") << std::endl;
	if (AEInputCheckTriggered(AEVK_SPACE) && rb->onCollider)
	{
		//moveDir.y = 500.f;
		//onGround = false;

		float jumpHeight = 300.0f; // pixels

		float jumpVelocity = sqrtf(2.0f * fabs(rb->gravity) * jumpHeight);

		rb->velocity.y = jumpVelocity;

	}
	//if (AEInputCheckCurr(AEVK_S))
	//{
	//	moveDir.y -= 1.f;
	//}
	float accel = 300.0f;     // acceleration power
	float decel = 400.0f;     // deceleration power
	float maxSpeed = 300.0f;
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

	if (rb->velocity.x != 0)
	{
		currentAction = rb->velocity.y != 0 ? PlayerAction::JUMPING : PlayerAction::RUNNING;
	}
	else {
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

	Sprite * s = new Sprite();
	s->meshColor = 0xFF0000FF;
	s->textureFileName = "Assets/SpriteSheets/test2x12.png";
	s->spriteSheet = Sprite::SpriteSheet(12, 2);
	s->spriteSheet.isSpriteSheet = true;

	idleAnim = new Animation(s);

	Sprite * run = new Sprite();
	run->meshColor = 0xFF0000FF;
	run->textureFileName = "Assets/SpriteSheets/testRed4x6.png";
	run->spriteSheet = Sprite::SpriteSheet(6, 4);
	run->spriteSheet.isSpriteSheet = true;

	runningAnim = new Animation(run);

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
	line = AddComponent(new Sprite()
	);
	line->spriteShape = SPRITE_SHAPE::SHAPE_LINE;
	line->thickness = 5.f;
	line->meshColor = 0xFFFF0000;
	playerLinePos = new Sprite::LinePoint;
	aimLinePos = new Sprite::LinePoint;
	playerLinePos->pos.x = pos.x;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = pos.x + 10.f;
	aimLinePos->pos.y = pos.y;
	line->linePoints.push_back(playerLinePos);
	line->linePoints.push_back(aimLinePos);

	showColliders = true;
	speed = static_cast<f32>(200.0);
	//AEVec2Set(&velocity, 0.f, 0.f);

	GameObject::Init();
}

void Player::Update(){
	
	//PlayerInput();
	PlayerAction();
	PlayerAnimation();
	s32 screenX, screenY;
	//f32 camPosX, camPosY;
	f32 worldPosX, worldPosY;
	AEInputGetCursorPosition(&screenX, &screenY);
	//AEGfxGetCamPosition(&camPosX, &camPosY);
	worldPosX = screenX + AEGfxGetWinMinX();
	worldPosY = -(screenY - AEGfxGetWinMaxY());
	std::cout << worldPosX << "   " << worldPosY << std::endl;
	playerLinePos->pos.x = pos.x;
	playerLinePos->pos.y = pos.y;
	aimLinePos->pos.x = worldPosX;
	aimLinePos->pos.y = worldPosY;
	//std::vector<Tile*> nearbyTiles = MapManager::GetTilesNearPos(pos, scale);
	//std::vector<Collider*> colliders = GetComponents<Collider>();

	//for (Collider* pCol : colliders)
	//{
	//	for (auto it = pCol->collisionInfos.begin(); it != pCol->collisionInfos.end(); )
	//	{
	//		Collider* oCol = it->other;

	//		if (!oCol || !oCol->canCollide)
	//		{
	//			pCol->RemoveFromOverlappingVector(oCol);
	//			it = pCol->collisionInfos.begin();
	//			continue;
	//		}

	//		if (BoxToBoxCollision(
	//			pCol->GetPos2D(), oCol->GetPos2D(),
	//			pCol->GetScale(), oCol->GetScale()))
	//		{
	//			PhysicsManager::HandleCollision(pCol, oCol);
	//			++it;
	//		}
	//		else
	//		{
	//			pCol->RemoveFromOverlappingVector(oCol);
	//			it = pCol->collisionInfos.begin();
	//		}
	//	}
	//}
	
	AEGfxSetCamPosition(pos.x, pos.y);
	GameObject::Update();
	std::cout << "Pos: " << pos.x << "   " << pos.y << "  Velocity: " << rb->velocity.x <<"  " << rb->velocity.y << std::endl;
}
