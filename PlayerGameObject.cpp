#include "AEEngine.h"
#include "PlayerGameObject.h"
#include "MapManager.h"
#include <iostream>
#include <vector>

void Player::Init()
{
	GameObject::Init();

	//set pos
	AEVec2Set(&pos,MapManager::GetPlayerSpawnPos().x, MapManager::GetPlayerSpawnPos().y);
	pos.z = 1.f;

	//set scale
	AEVec2Set(&scale, 37.5f, 37.5f);

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

	c->OnCollisionEnter = [](Collider * other) {
		std::cout << "Collision Enter" << std::endl;
		};
	c->OnCollisionOver = [](Collider * other) {
		//std::cout << "Collision Over" << std::endl;
		if (Tile* tile = dynamic_cast<Tile*>(other->owner))
		{
			if (tile->currID == TILE_ID::GROUND)
				std::cout << "Player on ground" << std::endl;
		}
	};	
	c->OnCollisionExit = [](Collider * other) {
		std::cout << "Collision Exit" << std::endl;
		};
	showColliders = true;
	speed = 50.f;
	AEVec2Set(&velocity, 0.f, 0.f);
}

void Player::Update(){
	GameObject::Update();

	AEVec2Set(&velocity, 0.f, 0.f);
	if (AEInputCheckCurr(AEVK_W))
	{
		//pos.y += speed * AEFrameRateControllerGetFrameTime();
		velocity.y = 1;
	}
	if (AEInputCheckCurr(AEVK_S))
	{
		//pos.y -= speed * AEFrameRateControllerGetFrameTime();
		velocity.y = -1;
	}
	if (AEInputCheckCurr(AEVK_A))
	{
		//pos.x -= speed * AEFrameRateControllerGetFrameTime();
		velocity.x = -1;
	}
	if (AEInputCheckCurr(AEVK_D))
	{
		//pos.x += speed * AEFrameRateControllerGetFrameTime();
		velocity.x = 1;
	}

	pos.x += velocity.x * speed * AEFrameRateControllerGetFrameTime();
	pos.y += velocity.y * speed * AEFrameRateControllerGetFrameTime();
}
