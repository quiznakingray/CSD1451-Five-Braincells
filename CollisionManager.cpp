#include "CollisionManager.h"

bool CheckBoxCollision(AEVec2 obj1, AEVec2 obj2)
{
	return false;
}

bool CheckCircleCollision(AEVec2 obj1, AEVec2 obj2)
{
	// Uses (dx^2+dy^2)≤(r1​+r2​)^2 to calculate collision of player and area
	f64 dx = obj1.x - obj2.x;
	f64 dy = obj1.y - obj2.y;
	f64 obj1Radius = obj1.x * 0.5;
	f64 obj2Radius = obj2.x * 0.5;
	f64 radiusSum = obj1Radius + obj2Radius;
	return (dx * dx + dy * dy) <= (radiusSum * radiusSum);
}
