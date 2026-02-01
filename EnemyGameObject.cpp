#include "EnemyGameObject.h"
#include "CollisionManager.h"
#include "MapManager.h"
#include "PlayerGameObject.h"
#include <iostream>

void EnemyGameObject::Init()
{
	GameObject::Init();

	InitEnemyBase(base, EnemyType::BASIC);
	InitEnemyMovement(movement);

	AEVec2Set(&pos, MapManager::GetPlayerSpawnPos().x + 50, MapManager::GetPlayerSpawnPos().y);
	pos.z = 1.f;

	AEVec2Set(&scale, 30.f, 30.f);

	base.patrolStart = { MapManager::GetPlayerSpawnPos().x, 200.f };
	base.patrolEnd = { 350.f, 200.f };

	Sprite* s = AddComponent(new Sprite());
	s->meshColor = 0xFF0000FF; // red enemy

	Collider* c = AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

	c->OnCollisionEnter = [this](Collider* other)
	{
		if (Player* player = dynamic_cast<Player*>(other->owner))
		{
			std::cout << "[EnemyGameObj] Collided with PLAYER\n";
		}
		else if (Tile* tile = dynamic_cast<Tile*>(other->owner))
		{
			std::cout << "[EnemyGameObj] Collided with TILE\n";
			movement.movingRight = !movement.movingRight;
		}
		else
		{
			std::cout << "[EnemyGameObj] Collided with UNKNOWN\n";
		}
	};

	showColliders = true;
}

void EnemyGameObject::Update()
{
	GameObject::Update();
	UpdateEnemyPatrol(base, movement, pos, AEFrameRateControllerGetFrameTime());
}
