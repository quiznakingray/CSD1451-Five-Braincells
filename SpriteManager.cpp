#include "SpriteManager.h"

void RenderShape(Shape shape, AEMtx33 transform) {
	TransformShape(shape.mesh, transform, shape);
	AEGfxSetTransform(transform.m);
	AEGfxSetColorToMultiply(0,
		0,
		0,
		0);
	// Set the color to add to nothing, so that we don't alter the sprite's color
	AEGfxSetColorToAdd((float)shape.color[0],
		(float)shape.color[1],
		(float)shape.color[2],
		(float)shape.color[3]);
	AEGfxTextureSet(shape.tex, 0, 0);
	// Tell Alpha Engine to draw the mesh with the above settings.
	AEGfxMeshDraw(shape.mesh, AE_GFX_MDM_TRIANGLES);
}

void TransformShape(AEGfxVertexList* mesh, AEMtx33& transform, Shape shape)
{
    // Calculate the final transform
    AEMtx33Identity(&transform);

    AEMtx33 rotation;
    AEMtx33Rot(&rotation, shape.degrees);

    AEMtx33 scale;
    AEMtx33Scale(&scale, shape.size.x, shape.size.y);

    AEMtx33 translate;
    AEMtx33Trans(&translate, shape.pos.x, shape.pos.y);

    // Concatenate everything
    AEMtx33Concat(&transform, &rotation, &scale);
    AEMtx33Concat(&transform, &translate, &transform);
}
