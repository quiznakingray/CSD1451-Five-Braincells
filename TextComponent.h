#ifndef TEXT_COMPONENT
#define TEXT_COMPONENT

#include "AEEngine.h"
#include "ComponentBase.h"
#include <string>
struct Text : ComponentBase {
	AEVec2 center{};
	f32 size = 1.f;

	bool inWorldSpace = false;
	const char * text{};

	void Render() override;

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
