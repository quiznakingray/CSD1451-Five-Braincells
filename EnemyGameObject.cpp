#include "EnemyGameObject.h"
#include "CollisionManager.h"
#include "MapManager.h"
#include "PlayerGameObject.h"
#include "PhysicsManager.h"
#include "GameStateManager.h"
#include "ParticleEffects.h"

#include <iostream>

void EnemyGameObject::Init()
{
	GameObject::Init();
	InitEnemyBase(base, EnemyType::BASIC);
	InitEnemyMovement(movement);

	Tile* spawnTile = MapManager::GetTile(TILE_ID::ENEMY);
	if (spawnTile == nullptr) {
		return;
	}
	AEVec2Set(&pos, spawnTile->pos.x + 50, spawnTile->pos.y);
	pos.z = 1.f;
	AEVec2Set(&scale, MapManager::tileSize, MapManager::tileSize);

	base.patrolStart = { spawnTile->pos.x - 100.f, spawnTile->pos.y - 100.f };
	base.patrolEnd = { spawnTile->pos.x + 150.f, spawnTile->pos.y - 100.f };

	Sprite* s = AddComponent(new Sprite());
	s->meshColor = 0xFF0000FF;

	Collider* c = AddComponent(new Collider(COLLIDER_TYPE::BOX_COLLIDER, 0, 0, 1, 1));

	c->OnCollisionEnter = [this](Collider* other, int sides)
		{
			// CHECK FOR ARROW COLLISION (Orange Blood)
			if (Arrow* arrow = dynamic_cast<Arrow*>(other->owner))
			{
				// Trigger the effect at the enemy's current position
				ParticleSystem::CreateBloodEffect(pos.x, pos.y);

				base.stats.health -= 10;
				std::cout << "Enemy hit! Health: " << base.stats.health << std::endl;

				// Optional: Deactivate the arrow so it doesn't pass through
				arrow->isActive = false;
			}
			else if (Player* player = dynamic_cast<Player*>(other->owner))
			{
				std::cout << "[EnemyGameObj] Collided with PLAYER\n";
				next = GAME_STATE_TYPE::COMBAT;
			}
			else if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				std::cout << "[EnemyGameObj] Collided with TILE\n";

				// Only flip direction if hitting a wall on the left or right
				// instead of always flipping (which caused flipping on ground too)
				if (sides & COLLISION_SIDE::LEFT || sides & COLLISION_SIDE::RIGHT)
					movement.movingRight = !movement.movingRight;
			}
			else
			{
				std::cout << "[EnemyGameObj] Collided with UNKNOWN\n";
			}
		};

	c->OnCollisionOver = [this](Collider* other, int sides)
		{
			if (Tile* tile = dynamic_cast<Tile*>(other->owner))
			{
				// Continuously correct direction if somehow pushed into a wall
				if (sides & COLLISION_SIDE::RIGHT && movement.movingRight)
					movement.movingRight = false;
				else if (sides & COLLISION_SIDE::LEFT && !movement.movingRight)
					movement.movingRight = true;
			}
		};

	rb = AddComponent(new RigidBody());
	showColliders = true;
}

void EnemyGameObject::Update()
{
	GameObject::Update();
	UpdateEnemyPatrol(this, AEFrameRateControllerGetFrameTime());

//	std::vector<Collider*> colliders = GetComponents<Collider>();
//
//	for (Collider* pCol : colliders)
//	{
//\
//		for (CollisionInfo& info : pCol->collisionInfos)
//		{
//			Collider* oCol = info.other; 
//
//			if (!oCol || !oCol->canCollide) continue;
//
//			if (BoxToBoxCollision(
//				pCol->GetPos2D(), oCol->GetPos2D(),
//				pCol->GetScale(), oCol->GetScale()))
//			{
//				PhysicsManager::HandleCollision(pCol, oCol);
//			}
//			else
//			{
//				pCol->RemoveFromOverlappingVector(oCol);
//				oCol->RemoveFromOverlappingVector(pCol);
//			}
//		}
//	}
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
