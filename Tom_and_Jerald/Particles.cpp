#include "pch.hpp"
#include "Playing.hpp"
#include "utils.hpp"
#include "Particles.hpp"



namespace graphics {

	//____________________________PARTICLE SYSTEM_____________________________
	void particleInit(float x, float y, float spawnAmount) {
		int spawned = 0;
		for (int i = 0; i < MAX_PARTICLES && spawned < spawnAmount; i++) {
			if (effects[i].lifetime <= 0) {
				effects[i].posX = x;
				effects[i].posY = y;
				effects[i].veloctyX = randomRange(-1.0f, 1.0f);
				effects[i].veloctyY = randomRange(-1.0f, 1.0f);
				effects[i].life = randomRange(40, 80);
				effects[i].size = randomRange(10, 20);

				spawned++;

			}
		}
	} //Spawn particles
	void particleDraw(void) {
		for (int i = 0; i < MAX_PARTICLES; i++) {
			if (effects[i].lifetime > 0) {
				effects[i].posX += effects[i].velocityX;
				effects[i].posY += effects[i].velocityY;
				effects[i].velocityY += 0.05f;
				effects[i].lifetime--;

				createUnitCircles();//create mesh
				drawQuad;
			}
		}
	} //Update particles


}
