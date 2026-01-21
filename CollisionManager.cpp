#include "CollisionManager.h"

bool CheckBoxCollision(AEVec2 obj1Pos, AEVec2 obj2Pos, AEVec2 obj1Size, AEVec2 obj2Size)
{
	// collision x-axis?
	bool collisionX = obj1Pos.x + obj2Size.x >= obj2Pos.x &&
		obj2Pos.x + obj2Size.x >= obj1Pos.x;
	// collision y-axis?
	bool collisionY = obj1Pos.y + obj1Size.y >= obj2Pos.y &&
		obj2Pos.y + obj2Size.y >= obj1Pos.y;
	// collision only if on both axes
	return collisionX && collisionY;
}