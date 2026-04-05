/*************************************************************************
@file    Particles.hpp
@Author  Loh Kai Xin kaixin.l@digipen.edu
@Co-authors  NIL
@brief
     Declares the particle system types and update/draw interface for
     jetpack thrust and explosion visual effects.

Copyright © 2026 DigiPen, All rights reserved.
*************************************************************************/
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

}
