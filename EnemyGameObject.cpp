#include "EnemyGameObject.h"
#include "CollisionManager.h"
#include "MapManager.h"
#include "PlayerGameObject.h"
#include "PhysicsManager.h"
#include "GameStateManager.h"
#include <iostream>

void EnemyGameObject::Init()
{
	GameObject::Init();

	InitEnemyBase(base, EnemyType::BASIC);
	InitEnemyMovement(movement);

	Tile* spawnTile = MapManager::GetTile(TILE_ID::ENEMY);
	AEVec2Set(&pos, spawnTile->pos.x + 50, spawnTile->pos.y);
	pos.z = 1.f;

	AEVec2Set(&scale, MapManager::tileSize , MapManager::tileSize );

	base.patrolStart = { spawnTile->pos.x - 100.f, spawnTile->pos.y - 100.f };
	base.patrolEnd = { spawnTile->pos.x + 150.f,spawnTile->pos.y - 100.f };

	Sprite* s = AddComponent(new Sprite());
	s->meshColor = 0xFF0000FF; // red enemy

	Collider* c = AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

	c->OnCollisionEnter = [this](Collider* other)
	{
		if (Player* player = dynamic_cast<Player*>(other->owner))
		{
			std::cout << "[EnemyGameObj] Collided with PLAYER\n";
			next = GAME_STATE_TYPE::COMBAT;

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

	rb = AddComponent(
		new RigidBody()
	);
	//rb->type = RIGIDBODY_TYPE::DYNAMIC;

	showColliders = true;
}

void EnemyGameObject::Update()
{
	GameObject::Update();
	//rb->velocity.x = -50.f;

	
	UpdateEnemyPatrol(this, AEFrameRateControllerGetFrameTime());

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
}

void EnemyGameObject::Render()
{
	GameObject::Render();


	// render patrol points

	GameObject* patrolStartGO = new GameObject(
		60.f, 60.f,
		base.patrolStart.x,
		base.patrolStart.y,
		1.f);

	Sprite* patrolStartSprite = patrolStartGO->AddComponent(
		new Sprite()
	);

	patrolStartSprite->meshColor = 0xFFFF0000;

	patrolStartGO->Init();
	patrolStartGO->Render();

	GameObject* patrolEndGO = new GameObject(
		60.f, 60.f,
		base.patrolEnd.x,
		base.patrolEnd.y,
		1.f);

	Sprite* patrolEndSprite = patrolEndGO->AddComponent(
		new Sprite()
	);

	patrolEndSprite->meshColor = 0xFFFF0000;

	patrolEndGO->Init();
	patrolEndGO->Render();
}
