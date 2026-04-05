//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H



#include <vector>
#include "ComponentBase.h"
#include "AeVec3.h"
#include "SpriteManager.h"

enum class STATE
{
	IDLE,
	WALK,
	JUMP,
	FALL
};

struct GameObject {
	AEVec3 pos{};
	AEVec3 posOnScreen{};
	AEVec2 scale{};
	f32 rotation{};

	bool isActive = true;
	bool showColliders = false;
	bool isOnCamera = true;
	bool isUI = false;
	//Sprite* sprite;
	STATE objectState = STATE::IDLE;
	
	std::vector<ComponentBase*> components{};

	template<typename T>
	T* AddComponent(T* component)
	{
		component->owner = this;
		components.push_back(component);
		//component->Init();
		return component;
	}


	template<typename T>
	std::vector<T*> GetComponents()
	{
		std::vector<T*> result;
		for (ComponentBase* c : components)
		{
			if (T* casted = dynamic_cast<T*>(c))
				result.push_back(casted);
		}
		return result;
	}
	template<typename T>
	T* GetComponent()
	{
		for (ComponentBase* c : components)
		{
			if (T* casted = dynamic_cast<T*>(c))
				return casted;
		}
		return nullptr;
	}
	template<typename T>
	bool RemoveComponent(T*& component)
	{
		for (auto it = components.begin(); it != components.end(); ++it)
		{
			if (*it == component)
			{
				delete* it;
				it = components.erase(it);
				component = nullptr;
				return true;
			}
		}
		return false;
	}
	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Free();

	GameObject() = default;
	virtual ~GameObject();
	GameObject(f32 scale_x, f32 scale_y, f32 pos_x, f32 pos_y, f32 pos_z = 0.f, f32 rot = 0.f, bool setUI = false)
	{
		AEVec2Set(&pos, pos_x, pos_y);
		pos.z = pos_z;
		AEVec2Set(&scale, scale_x, scale_y);
		rotation = rot;
		isUI = setUI;
	}

	bool isGameObjectOnScreen();

	GameObject(const AEVec3& pos, const AEVec3& posOnScreen, const AEVec2& scale, const f32& rotation, bool isActive, bool showColliders, bool isOnCamera, bool isUI, const STATE& objectState, const std::vector<ComponentBase*>& components)
		: pos(pos), posOnScreen(posOnScreen), scale(scale), rotation(rotation), isActive(isActive), showColliders(showColliders), isOnCamera(isOnCamera), isUI(isUI), objectState(objectState), components(components)
	{
	}
};
	
void AddGameObjectToVector(GameObject* go, std::vector<GameObject*>& gos);

void InitGameObjects(std::vector<GameObject*>& gos);
void UpdateGameObjects(std::vector<GameObject*>& gos);
void RenderGameObjects(std::vector<GameObject*>& gos);
void FreeGameObjects(std::vector<GameObject* >& gos);
void SetActiveGameObjects(std::vector<GameObject*>& vec, bool active);
#endif // !GAME_OBJECT_MANAGER_H