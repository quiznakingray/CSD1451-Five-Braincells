#pragma once
#include "AEEngine.h"
#include <array>

#define COLORSIZE 4
typedef struct Shape {
	AEVec2 pos = {};
	AEVec2 size{};
	f32 degrees{};
	std::array <f64, COLORSIZE > color{ };
	bool isCenter = false;
	AEGfxVertexList* mesh = nullptr;
	AEGfxTexture* tex = nullptr;
}Shape;


void RenderShape(Shape shape, AEMtx33 transform);
// Updates transform of shape
void TransformShape(AEGfxVertexList* mesh, AEMtx33& transform, Shape shape);
