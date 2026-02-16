#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H



#include <vector>
#include "ComponentBase.h"
#include "Utils.h"
#include "SpriteManager.h"

struct GameObject {
	AEVec3 pos{};
	AEVec2 scale{};
	f32 rotation{};

	bool isActive = true;
	bool showColliders = false;
	bool isOnCamera = true;
	//Sprite* sprite;
	
	std::vector<ComponentBase*> components{};

	template<typename T>
	T* AddComponent(T* component)
	{
		component->owner = this;
		components.push_back(component);
		component->Init();
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

	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void Free();

	GameObject() = default;
	virtual ~GameObject() {};
	GameObject(f32 scale_x, f32 scale_y, f32 pos_x, f32 pos_y, f32 pos_z = 0.f, f32 rot = 0.f)
	{
		AEVec2Set(&pos, pos_x, pos_y);
		pos.z = pos_z;
		AEVec2Set(&scale, scale_x, scale_y);
	}

	bool isGameObjectOnScreen();
};

void AddGameObjectToVector(GameObject* go, std::vector<GameObject*>& gos);

void InitGameObjects(std::vector<GameObject*>& gos);
void UpdateGameObjects(std::vector<GameObject*> &gos);
//void RenderGameObjects(std::vector<GameObject*>& gos);
#endif // !GAME_OBJECT_MANAGER_H