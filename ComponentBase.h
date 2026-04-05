//---------------------------------------------------------
// author:    Choy Phui Mun
//
// Copyright 2026 DigiPen, All rights reserved.
//---------------------------------------------------------
#ifndef COMPONENT_BASE_H
#define COMPONENT_BASE_H

struct GameObject;
struct ComponentBase {
	virtual ~ComponentBase() = default;

	// link to GameObbject 
	GameObject * owner = nullptr;
	bool isActive = true;
	// base component functions to be overridden by derived components
	virtual void Init() {};
	virtual void Update(){};
	virtual void Render(){};
	virtual void Free(){};
};
#endif // !COMPONENT_BASE