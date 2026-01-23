#include "pch.h"
#include "playing.h"

#ifndef UTILS_H
#define UTILS_H



void drawCenteredText(s8 font_id, const char* text, f32 y, f32 scale, f32 cam_pos_x = 0.0f, f32 cam_pos_y = 0.0f);

void createUnitSquare(AEGfxVertexList** out_mesh);
void createUnitCircles(AEGfxVertexList** out_mesh);

void drawQuad(AEGfxVertexList* mesh, f32 center_x, f32 center_y, f32 width, f32 height, f32 red, f32 green, f32 blue, f32 alpha);

bool checkOverlap(const AEVec2* position_a, const AEVec2* half_size_a, const AEVec2* position_b, const AEVec2* half_size_b);

f32 randomRange(f32 min_value, f32 max_value);

void drawHealthBar(AEGfxVertexList* mesh, const Player* player, int max_health);

/*// comment out first, will work on this later
na mespace graphics{

	//Structure for particle system
	struct particleSys {
		float posX;
		float posY;
		float scale;
		int lifetime;
		AEGfxMesh;
		AEGfxTexture;
	};


	//function for particle system
	void particleInit()
	void particleDraw()

	//Function for pixeltext
	void pixelTextInit()
	void pixelTextDraw()
}*/

#endif // !UTILS_H