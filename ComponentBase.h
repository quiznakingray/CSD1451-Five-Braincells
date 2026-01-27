#ifndef COMPONENT_BASE_H
#define COMPONENT_BASE_H

struct GameObject;
struct ComponentBase {
	virtual ~ComponentBase() = default;

	// link to GameObbject idk
	GameObject * owner = nullptr;

	virtual void Init() {};
	virtual void Update(){};
	virtual void Render(){};
	virtual void Free(){};
};
#endif // !COMPONENT_BASE