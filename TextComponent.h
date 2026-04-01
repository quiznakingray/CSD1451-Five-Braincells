#ifndef TEXT_COMPONENT
#define TEXT_COMPONENT

#include "AEEngine.h"
#include "ComponentBase.h"
#include <string>
struct Text : ComponentBase {
	AEVec2 center{};
	f32 size = 1.f;

	bool inWorldSpace = true;
	std::string textStr{};
	const char * text = "";

	void Render() override;
	void SetText(const std::string& str) {
		textStr = str;
		text = textStr.c_str();     // always valid as long as textStr lives
	}

	void Free() override;
};
#endif // !TEXT_COMPONENT
//struct TextManager {
//	static s8 pFont;
//	
//	static void Init()
//	{
//		pFont = AEGfxCreateFont("Assets/liberation-mono.ttf", 40);
//	}
//};
