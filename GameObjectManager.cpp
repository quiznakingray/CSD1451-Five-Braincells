#include "GameObjectManager.h"
#include "CollisionManager.h"
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
		comp->Update();
	}
}

void GameObject::Render() {
	for (ComponentBase* comp : components)
	{
		comp->Render();
	}
}
void GameObject::Free()
{
	for (ComponentBase* comp : components)
	{
		comp->Free();
	}

	components.clear();
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
	for (GameObject* firstGo : gos)
	{

		for (GameObject* secondGo : gos)
		{
			if (firstGo == secondGo) continue;

			// collision to collision 
			std::vector<Collider*> firstGoColliders = firstGo->GetComponents<Collider>();
			std::vector<Collider*> secondGoColliders = secondGo->GetComponents<Collider>();
			for (Collider* firstColl : firstGoColliders)
			{
				if (!firstColl->canCollide) continue;
				for (Collider* secondColl : secondGoColliders)
				{
					if (!secondColl->canCollide) continue;
					// check if there is collision between both colliders
					bool colliding = false;
					if (firstColl->type == COLLIDER_TYPE::BOX_COLLIDER
						&& secondColl->type == COLLIDER_TYPE::BOX_COLLIDER)
					{
						colliding = BoxToBoxCollision(
							firstColl->GetPos2D(), secondColl->GetPos2D(),
							firstColl->GetScale(), secondColl->GetScale()
						);
					}
					else {
						// circle to circle
						// circle to box

					}

					if (colliding)
					{
						//std::cout << "colliding" << std::endl;

						//Add to list
						firstColl->AddToOvelappingVector(secondColl);
						secondColl->AddToOvelappingVector(firstColl);

						if (!firstColl->isTrigger && !secondColl->isTrigger)
						{

						}
					}
					else {
						//remove from list
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
		for (Collider* col : go->GetComponents<Collider>())
		{
			if (!col->canInteract) continue;
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
			if (!col->canInteract) continue;

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
void UpdateGameObjects(std::vector<GameObject*> &gos)
{
	for (GameObject* firstGo : gos)
	{
		firstGo->Update();
	}
	HandleCollision(gos);
	HandleInteraction(gos);
}

void RenderGameObjects(std::vector<GameObject*>& gos)
{
	for (GameObject* go : gos)
	{
		go->Render();
	}
}
