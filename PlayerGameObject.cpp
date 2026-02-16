#include "AEEngine.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include <iostream>
#include <vector>

void Player::PlayerInput()
{
	AEVec2 moveDir{};
	//AEVec2Set(&velocity, 0.f, 0.f);
	//std::cout << "On ground: " << (onGround ? "--" : "___________________________ ") << std::endl;
	if (AEInputCheckTriggered(AEVK_SPACE) && rb->onCollider)
	{
		moveDir.y = 500.f;
		//onGround = false;

	}
	if (AEInputCheckCurr(AEVK_S))
	{
		moveDir.y -= 1.f;
	}
	if (AEInputCheckCurr(AEVK_A))
	{
		moveDir.x -= 1.f;  
	}
	if (AEInputCheckCurr(AEVK_D))
	{
		moveDir.x += 1.f;
	}

	float length = sqrt(moveDir.x * moveDir.x);
	if (length > 0) {
		moveDir.x /= length;
	}

	// Set velocity
	rb->velocity.x = moveDir.x * speed;
	rb->velocity.y += moveDir.y ;

}

void Player::Init()
{

	//set pos
	AEVec2Set(&pos,MapManager::GetPlayerSpawnPos().x, MapManager::GetPlayerSpawnPos().y + 100.f);
	pos.z = 1.f;

	//set scale
	AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);

	// set components
	Sprite * s = AddComponent(
		new Sprite());
	s->meshColor = 0xFF0000FF;

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

	c->OnCollisionEnter = [this](Collider * other) {
		//std::cout << "Collision Enter" << std::endl;
		if (Tile * tile = dynamic_cast<Tile*>(other->owner))
		{
			//this->rb->onCollider = true;
		}
	};
	c->OnCollisionOver = [this](Collider * other) {
		//std::cout << "Collision Over" << std::endl;
		//if (Tile* tile = dynamic_cast<Tile*>(other->owner))
		//{
		//	if (tile->currID == TILE_ID::GROUND)
		//		std::cout << "Player on ground" << std::endl;
		//}
	};	
	c->OnCollisionExit = [this](Collider * other) {
		//std::cout << "Collision Exit" << std::endl;
		if (Tile* tile = dynamic_cast<Tile*>(other->owner))
		{
			this->rb->onCollider = false;
		}
	};

	rb = AddComponent(
		new RigidBody()
	);
	rb->type = RIGIDBODY_TYPE::DYNAMIC;

	//showColliders = true;
	speed = 200.f;
	//AEVec2Set(&velocity, 0.f, 0.f);
	AEGfxSetCamPosition(pos.x, pos.y);

	GameObject::Init();
}

void Player::Update(){

	PlayerInput();
	//std::vector<Tile*> nearbyTiles = MapManager::GetTilesNearPos(pos, scale);
	std::vector<Collider*> colliders = GetComponents<Collider>();

	for (Collider* pCol : colliders)
	{
		for (Collider* oCol : pCol->overlappingColliders)
		{

			if (BoxToBoxCollision(
				pCol->GetPos2D(), oCol->GetPos2D(),
				pCol->GetScale(), oCol->GetScale()))
			{
				//pCol->AddToOvelappingVector(oCol);
				//oCol->AddToOvelappingVector(pCol);
				PhysicsManager::HandleCollision(pCol, oCol);
			}
			else {
				//pCol->RemoveFromOverlappingVector(oCol);
				//oCol->RemoveFromOverlappingVector(pCol);
			}
		
		}
	}


	AEGfxSetCamPosition(pos.x, pos.y);
	GameObject::Update();
}
