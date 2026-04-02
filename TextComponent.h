#ifndef TEXT_COMPONENT
#define TEXT_COMPONENT

#include "AEEngine.h"
#include "ComponentBase.h"
#include "SpriteManager.h"
#include <string>
struct Text : ComponentBase {
	AEVec2 center{};
	f32 size = 1.f;
	Color color = {1.f, 1.f, 1.f, 1.f}; // default white

	bool inWorldSpace = true;
	std::string textStr{};
	const char * text = "";

	void Render() override;
	void SetText(const std::string& str) {
		textStr = str;
		text = textStr.c_str();     // always valid as long as textStr lives
	}
	void SetColor(Color rgba) {
		color = rgba;
	}

	void Free() override;
};
#endif 
