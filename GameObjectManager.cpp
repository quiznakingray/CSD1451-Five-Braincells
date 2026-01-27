#include "GameObjectManager.h"
#include "CollisionManager.h"
#include <type_traits>
#include <iostream>

void GameObject::Init()
{


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

void UpdateGameObjects(std::vector<GameObject*> gos)
{
	for (GameObject* go : gos)
	{
		go->Update();

		for (GameObject* go1 : gos)
		{
			if (go == go1) continue;


		}
		
		
	}
}
