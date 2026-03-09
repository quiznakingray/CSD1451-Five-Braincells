#include "GameObjectManager.h"
#include "CollisionManager.h"
#include "PhysicsManager.h"
#include "MapManager.h"
#include <type_traits>
#include <iostream>
#include <algorithm>



void GameObject::Init()
{
	for (ComponentBase* comp : components)
	{
		comp->Init();
	}
}
void GameObject::Update()
{
	for (ComponentBase*  comp: components)
	{
		//if (Collider * collider = dynamic_cast<Collider*>(comp))
		//{
		//	std::cout << "This is a collider" << std::endl;
		//}
		if (!comp->isActive) continue;
		comp->Update();
	}
}

void GameObject::Render() {
	if (!isOnCamera || !isActive) return;
	for (ComponentBase* comp : components)
	{
		if (!comp->isActive) return;
		comp->Render();
	}
}
void GameObject::Free()
{
	for (ComponentBase* comp : components)
	{
		comp->Free();
		delete comp;
	}

	components.clear();
}

void HandleState(std::vector<GameObject*>& gos)
{
	for (size_t i = 0; i < gos.size(); ++i)
	{
		GameObject* go = gos[i];
		
		RigidBody* rb = go->GetComponent<RigidBody>();
		if (!go->isOnCamera || !go->isActive || rb == nullptr) continue;
		if (rb->velocity.x == rb->prevVel.x && rb->velocity.y == rb->prevVel.y)
			return; // no change, skip state update
		if (rb->velocity.y < 0) {
			go->objectState = STATE::FALL;
		}
		else if (rb->velocity.y > 0) {
			go->objectState = STATE::JUMP;
		}
		else {
			if (fabs(rb->velocity.x) > 0.1f)
				go->objectState = STATE::WALK;
			else
				go->objectState = STATE::IDLE;
		}
		rb->prevVel = rb->velocity;
	}
}

bool GameObject::isGameObjectOnScreen()
{
	f32 camX, camY, camMinX, camMinY, camMaxX, camMaxY;
	f32 windowWidth = AEGfxGetWindowWidth();
	f32 windowHeight = AEGfxGetWindowHeight();
	AEGfxGetCamPosition(&camX, &camY);
	camMinX = camX - windowWidth / 2.f;
	camMaxX = camX + windowWidth / 2.f;
	camMinY = camY - windowHeight / 2.f;
	camMaxY = camY + windowHeight / 2.f;

	//player pos
	f32 posMinX, posMinY, posMaxX, posMaxY;
	posMinX = pos.x - scale.x / 2.f;
	posMaxX = pos.x + scale.x / 2.f;
	posMinY = pos.y - scale.y / 2.f;
	posMaxY = pos.y + scale.y / 2.f;


	return !(posMaxX < camMinX || posMinX > camMaxX ||
		posMaxY < camMinY || posMinY > camMaxY);
}


void AddGameObjectToVector(GameObject* go, std::vector<GameObject*>& gos)
{
	gos.push_back(go);

	// sort array
	std::sort(gos.begin(), gos.end(),
		[](GameObject* a, GameObject* b)
		{
			return a->pos.z < b->pos.z;
		}
	);

}

void HandleCollision(std::vector<GameObject*>& gos)
{
	for (size_t i = 0; i < gos.size(); ++i)
	{
		GameObject* firstGo = gos[i];
		if (!firstGo->isOnCamera || !firstGo->isActive) continue;

		auto isDynamic = [](GameObject* go) {
			RigidBody* rb = go->GetComponent<RigidBody>();
			return rb && rb->type == RIGIDBODY_TYPE::DYNAMIC;
			};
		// skip static vs static + cloudTile + crateTile entirely
		bool firstIsStatic = dynamic_cast<Tile*>(firstGo) && !isDynamic(firstGo);


		for (size_t j = i + 1; j < gos.size(); ++j)
		{
			GameObject* secondGo = gos[j];
			if (!secondGo->isOnCamera || !secondGo->isActive) continue;

			bool secondIsStatic = dynamic_cast<Tile*>(secondGo) && !isDynamic(secondGo);

			// two static tiles never need collision checks
			if (firstIsStatic && secondIsStatic) continue;

			std::vector<Collider*> firstGoColliders = firstGo->GetComponents<Collider>();
			std::vector<Collider*> secondGoColliders = secondGo->GetComponents<Collider>();

			for (Collider* firstColl : firstGoColliders)
			{
				if (!firstColl->canCollide) continue;
				for (Collider* secondColl : secondGoColliders)
				{
					if (!secondColl->canCollide) continue;

					bool colliding = false;
					if (firstColl->type == COLLIDER_TYPE::BOX_COLLIDER
						&& secondColl->type == COLLIDER_TYPE::BOX_COLLIDER)
					{
						colliding = BoxToBoxCollision(
							firstColl->GetPos2D(), secondColl->GetPos2D(),
							firstColl->GetScale(), secondColl->GetScale()
						);
					}

					if (colliding)
					{
						int sidesForFirst = GetAllCollisionSides(
							firstColl->GetPos2D(), secondColl->GetPos2D(),
							firstColl->GetScale(), secondColl->GetScale()
						);
						int sidesForSecond = FlipCollisionSides(sidesForFirst);
						firstColl->AddToOvelappingVector(secondColl, sidesForFirst);
						secondColl->AddToOvelappingVector(firstColl, sidesForSecond);
					}
					else
					{
						firstColl->RemoveFromOverlappingVector(secondColl);
						secondColl->RemoveFromOverlappingVector(firstColl);
					}
				}
			}
		}
	}
}

void HandleInteraction(std::vector<GameObject*>& gos)
{
	GameObject* topGO = nullptr;
	Collider* topCollider = nullptr;
	f32 highestZ = -1.f;

	// Find topmost GameObject under cursor
	for (GameObject* go : gos)
	{
		if (!go->isActive) continue;
		for (Collider* col : go->GetComponents<Collider>())
		{
			if (!col->canInteract || !go->isOnCamera) continue;
			if (IsCursorOverRect(
				go->pos.x + col->center.x,
				go->pos.y + col->center.y,
				go->scale.x * col->size.x,
				go->scale.y * col->size.y))
			{
				if (go->pos.z > highestZ)
				{
					highestZ = go->pos.z;
					topGO = go;
					topCollider = col;
				}
			}
		}
	}


	// Update colliders
	for (GameObject* go : gos)
	{
		for (Collider* col : go->GetComponents<Collider>())
		{
			if (!col->canInteract || !go->isOnCamera) continue;

			bool isHover = (col == topCollider);

			if (!col->isHovering && isHover)
			{
				if (col->OnMouseEnter) col->OnMouseEnter();
			}
			if (col->isHovering && !isHover)
			{
				if(col->OnMouseExit) col->OnMouseExit();
			}

			if (isHover)
			{
				if (col->OnMouseOver) col->OnMouseOver();
				if (AEInputCheckTriggered(AEVK_LBUTTON) && !col->isInteracting)
				{
					if (col->OnMouseDown) col->OnMouseDown();
					col->isInteracting = true;
				}
			}

			if (col->isInteracting)
			{
				if (AEInputCheckCurr(AEVK_LBUTTON))
				{
					if (col->OnClick) col->OnClick();
				}
				else if (AEInputCheckReleased(AEVK_LBUTTON))
				{
					if (col->OnMouseUp) col->OnMouseUp();
					col->isInteracting = false;
				}
			}
			col->isHovering = isHover;


		}

	}

	
}
void InitGameObjects(std::vector<GameObject*>& gos)
{
	for (GameObject* go : gos)
	{
		go->Init();
	}
}
void UpdateGameObjects(std::vector<GameObject*> &gos)
{
	for (GameObject* firstGo : gos)
	{
		if (!firstGo->isActive) continue;
		firstGo->Update();
		firstGo->isOnCamera = firstGo->isGameObjectOnScreen();
	}
	HandleCollision(gos);
	HandleInteraction(gos);
	HandleState(gos);
}
//void RenderGameObjects(std::vector<GameObject*>& gos)
//{
//	for (GameObject* go : gos)
//	{
//		if (!go->isActive) continue;
//		go->Render();
//	}
//}
