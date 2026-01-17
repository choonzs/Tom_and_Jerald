#include "pch.h"


void drawCenteredText(s8 font_id, const char* text, f32 y, f32 scale)
{
	f32 width = 0.0f;
	f32 height = 0.0f;
	AEGfxGetPrintSize(font_id, text, scale, &width, &height);
	AEGfxPrint(font_id, text, -width * 0.5f, y, scale, 1.0f, 1.0f, 1.0f, 1.0f);
}

void createUnitSquare(AEGfxVertexList** out_mesh)
{
	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 1.0f);
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 1.0f);
	*out_mesh = AEGfxMeshEnd();
}

void drawQuad(AEGfxVertexList* mesh, f32 center_x, f32 center_y, f32 width, f32 height, f32 red, f32 green, f32 blue, f32 alpha)
{
	AEMtx33 scale;
	AEMtx33 translate;
	AEMtx33 transform;

	AEMtx33Scale(&scale, width, height);
	AEMtx33Trans(&translate, center_x, center_y);
	AEMtx33Concat(&transform, &translate, &scale);

	AEGfxSetTransform(transform.m);
	AEGfxSetColorToMultiply(red, green, blue, alpha);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

bool checkOverlap(const AEVec2* position_a, const AEVec2* half_size_a, const AEVec2* position_b, const AEVec2* half_size_b)
{
	if (fabsf(position_a->x - position_b->x) > (half_size_a->x + half_size_b->x))
		return false;

	if (fabsf(position_a->y - position_b->y) > (half_size_a->y + half_size_b->y))
		return false;

	return true;
}

f32 randomRange(f32 min_value, f32 max_value)
{
	return min_value + (max_value - min_value) * (rand() / (f32)RAND_MAX);
}

