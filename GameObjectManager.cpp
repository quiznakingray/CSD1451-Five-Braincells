#include "GameObjectManager.h"
#include "CollisionManager.h"
#include "PhysicsManager.h"
#include "MapManager.h"
#include "CameraSystem.h"
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
		if (!comp->isActive) continue;
		comp->Update();
	}
	posOnScreen.x = pos.x + CameraSystem::GetCameraPos().x;
	posOnScreen.y = pos.y + CameraSystem::GetCameraPos().y;
	posOnScreen.z = pos.z;
}

void GameObject::Render() {
	if (!isOnCamera || !isActive) return;
	for (ComponentBase* comp : components)
	{
		if (!comp->isActive) continue;
		comp->Render();
	}
}
void GameObject::Free()
{
	for (ComponentBase* comp : components)
	{
		comp->Free(); 
	}

}
GameObject::~GameObject() {
	for (ComponentBase* comp : components)
	{
		delete comp; // memory cleanup only
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
	f32 windowWidth = static_cast<f32>(AEGfxGetWindowWidth());
	f32 windowHeight = static_cast<f32>(AEGfxGetWindowHeight());
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
	if (std::find(gos.begin(), gos.end(), go) != gos.end()) return;
	gos.push_back(go);

	// sort array
	std::sort(gos.begin(), gos.end(),
		[](GameObject* a, GameObject* b)
		{
			return a->pos.z < b->pos.z;
		}
	);

}

struct CollisionCandidate
{
	GameObject* go;
	std::vector<Collider*>  colliders;
	bool                    isStatic;
};

void HandleCollision(std::vector<GameObject*>& gos)
{
	// --- Broad phase: build candidate list once, O(n) ---
	std::vector<CollisionCandidate> candidates;
	candidates.reserve(gos.size());

	for (GameObject* go : gos)
	{
		if (!go->isActive) continue;

		std::vector<Collider*> cols = go->GetComponents<Collider>();
		// strip non-collidable up front
		cols.erase(
			std::remove_if(cols.begin(), cols.end(),
				[](Collider* c) { return !c->canCollide; }),
			cols.end()
		);
		if (cols.empty()) continue; // no point testing this object at all

		RigidBody* rb = go->GetComponent<RigidBody>();
		bool isDynamic = rb && rb->type == RIGIDBODY_TYPE::DYNAMIC;
		bool isStaticTile = dynamic_cast<Tile*>(go) && !isDynamic;

		candidates.push_back({ go, std::move(cols), isStaticTile });
	}

	// --- Narrow phase: O(n²) but on a much smaller, pre-filtered list ---
	for (size_t i = 0; i < candidates.size(); ++i)
	{
		CollisionCandidate& first = candidates[i];

		for (size_t j = i + 1; j < candidates.size(); ++j)
		{
			CollisionCandidate& second = candidates[j];

			// Static vs static: never collide
			if (first.isStatic && second.isStatic) continue;

			for (Collider* firstColl : first.colliders)
			{
				for (Collider* secondColl : second.colliders)
				{
					bool colliding = false;

					COLLIDER_TYPE ft = firstColl->type;
					COLLIDER_TYPE st = secondColl->type;

					if (ft == COLLIDER_TYPE::BOX_COLLIDER && st == COLLIDER_TYPE::BOX_COLLIDER)
					{
						colliding = BoxToBoxCollision(
							firstColl->GetPos2D(), secondColl->GetPos2D(),
							firstColl->GetScale(), secondColl->GetScale()
						);
					}
					else if ((ft == COLLIDER_TYPE::BOX_COLLIDER && st == COLLIDER_TYPE::CIRCLE_COLLIDER) ||
						(ft == COLLIDER_TYPE::CIRCLE_COLLIDER && st == COLLIDER_TYPE::BOX_COLLIDER))
					{
						Collider* box = (ft == COLLIDER_TYPE::BOX_COLLIDER) ? firstColl : secondColl;
						Collider* circle = (ft == COLLIDER_TYPE::CIRCLE_COLLIDER) ? firstColl : secondColl;
						colliding = BoxToCircleCollision(
							box->GetPos2D(), circle->GetPos2D(),
							box->GetScale(), circle->GetScale()
						);
					}
					else
					{
						// circle vs circle
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
	struct InteractCandidate
	{
		GameObject* go;
		Collider* col;
	};

	std::vector<InteractCandidate> candidates;
	candidates.reserve(gos.size());

	for (size_t i = 0; i < gos.size(); ++i)
	{
		GameObject* go = gos[i];
		if (!go->isActive || !go->isOnCamera) continue;
		std::vector<Collider*> cols = go->GetComponents<Collider>();
		for (size_t j = 0; j < cols.size(); ++j)
		{
			Collider* col = cols[j];
			if (!col->canInteract) continue;
			InteractCandidate c;
			c.go = go;
			c.col = col;
			candidates.push_back(c);
		}
	}

	// --- Find topmost under cursor ---
	GameObject* topGO = nullptr;
	Collider* topCollider = nullptr;
	f32         highestZ = -1.f;

	for (size_t i = 0; i < candidates.size(); ++i)
	{
		GameObject* go = candidates[i].go;
		Collider* col = candidates[i].col;
		if (IsCursorOverRect(
			go->posOnScreen.x + col->center.x,
			go->posOnScreen.y + col->center.y,
			go->scale.x * col->size.x,
			go->scale.y * col->size.y))
		{
			if (go->posOnScreen.z > highestZ)
			{
				highestZ = go->posOnScreen.z;
				topGO = go;
				topCollider = col;
			}
		}
	}

	// --- Hoist input checks ---
	bool lmbTriggered = AEInputCheckTriggered(AEVK_LBUTTON);
	bool lmbHeld = AEInputCheckCurr(AEVK_LBUTTON);
	bool lmbReleased = AEInputCheckReleased(AEVK_LBUTTON);

	// --- Update interaction state ---
	for (size_t i = 0; i < candidates.size(); ++i)
	{
		GameObject* go = candidates[i].go;
		Collider* col = candidates[i].col;
		bool isHover = (col == topCollider);
		if (!col) continue;
		if (col->isHovering == false && isHover)
			if (col->OnMouseEnter) col->OnMouseEnter();

		if (col->isHovering == true && !isHover)
			if (col->OnMouseExit) col->OnMouseExit();

		if (isHover)
		{
			if (col->OnMouseOver) col->OnMouseOver();
			if (lmbTriggered && !col->isInteracting)
			{
				if (col->OnMouseDown) col->OnMouseDown();
				col->isInteracting = true;
			}
		}

		if (col->isInteracting)
		{
			if (lmbHeld)
			{
				if (col->OnClick) col->OnClick();
			}
			else if (lmbReleased)
			{
				if (col->OnMouseUp) col->OnMouseUp();
				col->isInteracting = false;
			}
		}

		col->isHovering = isHover;
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
		if (!firstGo->isUI) firstGo->isOnCamera = firstGo->isGameObjectOnScreen();
		firstGo->Update();
	}
	HandleCollision(gos);
	HandleInteraction(gos);
	HandleState(gos);
}
void RenderGameObjects(std::vector<GameObject*>& gos)
{
	for (GameObject* go : gos)
	{
		if (!go->isActive) continue;
		go->Render();
	}
}

void FreeGameObjects(std::vector<GameObject*>& gos)
{
	for (GameObject* go : gos)
	{
		go->Free();
	}
}

void SetActiveGameObjects(std::vector<GameObject*>& vec, bool active)
{
	for (GameObject* obj : vec)
		obj->isActive = active;
}