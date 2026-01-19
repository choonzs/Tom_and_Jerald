#include "pch.h"

#ifndef UTILS_H
#define UTILS_H



void drawCenteredText(s8 font_id, const char* text, f32 y, f32 scale);

void createUnitSquare(AEGfxVertexList** out_mesh);
void createUnitCircles(AEGfxVertexList** out_mesh);

void drawQuad(AEGfxVertexList* mesh, f32 center_x, f32 center_y, f32 width, f32 height, f32 red, f32 green, f32 blue, f32 alpha);

bool checkOverlap(const AEVec2* position_a, const AEVec2* half_size_a, const AEVec2* position_b, const AEVec2* half_size_b);

f32 randomRange(f32 min_value, f32 max_value);

#endif // !UTILS_H