#include "PlayerGameObject.h"
#include <iostream>

void Player::Init()
{
	GameObject::Init();

	//set pos
	AEVec2Set(&pos, 0.f, 0.f);
	pos.z = 1.f;

	//set scale
	AEVec2Set(&scale, 100.f, 100.f);

	// set components
	Sprite * s = AddComponent(
		new Sprite(100.f, 100.f, 0.f, 0.f, 0.f, 0.f));


	Collider * c = AddComponent(
		new Collider(COLLIDER_TYPE::BOX_COLLIDER, 25)
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

	c->OnCollisionEnter = [] {
		std::cout << "Collision Enter" << std::endl;
		};
	c->OnCollisionOver = [] {
		std::cout << "Collision Over" << std::endl;
		};	
	c->OnCollisionExit = [] {
		std::cout << "Collision Exit" << std::endl;
		};
	showColliders = true;
	speed = 5.f;
}

void Player::Update(){
	GameObject::Update();

	if (AEInputCheckCurr(AEVK_W))
	{
		pos.y += speed;
	}
	if (AEInputCheckCurr(AEVK_S))
	{
		pos.y -= speed;
	}
	if (AEInputCheckCurr(AEVK_A))
	{
		pos.x -= speed;
	}
	if (AEInputCheckCurr(AEVK_D))
	{
		pos.x += speed;
	}
}
