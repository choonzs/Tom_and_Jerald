#include "pch.hpp"
#include "Playing.hpp"




namespace graphics {

	//Structure for particle system
	struct particleSys {
		float posX;
		float posY;
		float velocityX;
		float velocityY;
		float size;
		float lifetime;
	};

#define MAX_PARTICLES 200
	particleSys effects[MAX_PARTICLES];

	//function for particle system
	void particleInit(float x, float y, float spawnAmount); //Spawn particles
	void particleDraw(void); //Update particles

	//Function for pixeltext
	//void pixelTextInit();
	//void pixelTextDraw();
}
