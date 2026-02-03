#include "pch.hpp"
#include "Playing.hpp"
#include "utils.hpp"
#include "Particles.hpp"

namespace graphics {

	particleSys effects[MAX_PARTICLES];  // definition

	//____________________________PARTICLE SYSTEM_____________________________
	void particleInit(float x, float y, int spawnAmount) {
		int spawned = 0;

		for (int i = 0; i < MAX_PARTICLES && spawned < spawnAmount; i++) {
			if (effects[i].lifetime <= 0) {
				effects[i].posX = x;
				effects[i].posY = y;

				effects[i].velocityX = randomRange(-1.0f, 1.0f);
				effects[i].velocityY = randomRange(-1.0f, 1.0f);

				effects[i].lifetime = randomRange(40, 80);
				effects[i].size = randomRange(10.0f, 20.0f);

				// Optional: random color per particle
				//effects[i].r = randomRange(0.8f, 1.0f);
				//effects[i].g = randomRange(0.4f, 0.7f);
				//effects[i].b = randomRange(0.1f, 0.3f);
				//effects[i].a = 1.0f;

				spawned++;
			}
		}
	} // Spawn particles


	void particleDraw(AEGfxVertexList* mesh) {
		for (int i = 0; i < MAX_PARTICLES; i++) {
			if (effects[i].lifetime > 0) {

				// Update physics
				effects[i].posX += effects[i].velocityX;
				effects[i].posY += effects[i].velocityY;
				effects[i].velocityY += 0.05f;   // gravity
				effects[i].lifetime--;

				// Optional fade-out
				//effects[i].a -= 1.0f / 80.0f;
				//if (effects[i].a < 0.0f) effects[i].a = 0.0f;

				// Draw particle
				drawQuad(
					mesh,
					effects[i].posX,
					effects[i].posY,
					effects[i].size,
					effects[i].size,
					1.0f, 1.0f, 1.0f,     // color (white)
					1.0f);                //Alpha
			}
		}
	} // Update & draw particles

}