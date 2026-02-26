#include "utils_collision_check.h"

bool IsCursorOverRect(f32 pos_x, f32 pos_y, f32 scale_x, f32 scale_y)
{
	s32 screenMouseX, screenMouseY;
	AEInputGetCursorPosition(&screenMouseX, &screenMouseY);
	
	s32 mouseX = screenMouseX - AEGfxGetWindowWidth() / 2.f, mouseY = -(screenMouseY - AEGfxGetWindowHeight() / 2.f);

	f32 minX = pos_x - scale_x / 2.f;
	f32 maxX = pos_x + scale_x / 2.f;

	f32 minY = pos_y - scale_y / 2.f;
	f32 maxY = pos_y + scale_y / 2.f;

	return mouseX >= minX && mouseX <= maxX 
		&& mouseY >= minY && mouseY <= maxY;

}
