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
	extern particleSys effects[MAX_PARTICLES];  // declaration only

	//function for particle system
	void particleInit(float x, float y, int spawnAmount); //Spawn particles
	void particleDraw(AEGfxVertexList* circle); //Update particles

	//Function for pixeltext
	//void pixelTextInit();
	//void pixelTextDraw();
}
