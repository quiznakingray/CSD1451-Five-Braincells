#ifndef GAME_OBJECT_MANAGER_H
#define GAME_OBJECT_MANAGER_H



#include <vector>

#include "Utils.h"
#include "SpriteManager.h"
#include "ComponentBase.h"

struct GameObject {
	AEVec3 pos{};
	AEVec2 scale{};
	f32 rotation{};

	bool isActive = true;
	bool showColliders = false;
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
			if (auto* casted = dynamic_cast<T*>(c))
				result.push_back(casted);
		}
		return result;
	}
	void Init();
	void Update();
	void Render();
	void Free();

	GameObject(f32 scale_x, f32 scale_y, f32 pos_x, f32 pos_y, f32 pos_z = 0.f, f32 rot = 0.f)
	{
		AEVec2Set(&pos, pos_x, pos_y);
		pos.z = pos_z;
		AEVec2Set(&scale, scale_x, scale_y);
	}

};

void UpdateGameObjects(std::vector<GameObject*> gos);
#endif // !GAME_OBJECT_MANAGER_H