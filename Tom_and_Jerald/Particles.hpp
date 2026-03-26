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

	

	/*
	//DONT PUSH THIS FIRST UNTIL U FINISH, THEN DELETE THE PREVIOUS CODE
	class ParticleSystem {

	public: //data members , SET DEFAULT VALUES
	f32 posX;
	f32 posY;
	f32 velX;
	f32 velY;
	f32 size;
	f32 lifetime;

	f32 red;
	f32 green;
	f32 blue;
	f32 alpha;


	private:
		std::list<std::string> index{ "Fire","Smoke","","" }; //To choose specific file part to read,ADD INDEX HERE

	public: //Data functions
	 //Initialize particles
      void Particle_Init(f32 Size, f32 R, f32 G, f32 B, f32 Alpha);

    //Read information from external file and pass to Anim_Init to initialize.
	  bool ImportFromFile(const char* filename, std::string index);

    //Set up specific particle instance/type
    void Particle_Select();

    //Updates particles to run
    void Particle_Update(f32 delta_time);

    //Renders the particles
    void Particle_Draw
    (AEGfxTexture* Texture) const;

	//Extern instances here



	}//End PARTICLESYSTEM
	
	*/
	
	
	
	
	
	
}
