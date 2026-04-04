#pragma once //Header guard
#include "pch.hpp"

namespace ANIMATION {
    class AnimatedSprite {
    public: //Variables_________________________________
        //For spritesheet layout________________________
        //Set with in-class defaults, safeguards for Anim_Init()
        u32 rows{ 1 }, cols{ 1 };                   //Rows + columns in file
        u32 max_sprites{ 1 };                       //Total frames(rows*cols)
        f32 uv_width{ 1.0f }, uv_height{ 1.0f };    //UV width and height of 1 frame
        //For playback state____________________________
        f32 timer{ 0.0f };                          //Accumilate delta time per frame
        f32 duration_per_frame{ 0.6f };             //Seconds displayed per frame, 0.1f is 10fps
        u32 active_row{ 0 };                        //Spritesheet row that animaton is on
        u32 col_start{ 0 };                         //Spritesheet column that animation starts
        u32 col_count{ 1 };                         //Amount of frames the animaton clip have 
        u32 current_index{ 0 };                     //Index tracker within current clip, goes from 0 to col_count - 1
        //For UV offset_________________________________
        f32 uv_offset_x{ 0.0f }, uv_offset_y{ 0.0f };//Final UV coords, read these when drawing, pass to texture set

    public: //Member functions__________________________
        //Initialize spritesheet
        void Anim_Init(u32 total_rows, u32 total_cols);

        //Read information from external file and pass to Anim_Init to initialize.
        bool ImportFromFile(const char* filename);

        //Set up specific animation clip
        void Clip_Select(u32 row, u32 start_col, u32 frame_count, f32 fps = 2.0f);

        //Updates animation clip to run
        void Anim_Update(f32 delta_time);

        //Renders the clip
        void Anim_Draw(AEGfxTexture* Texture) const;

    private: //Helper functions_________________________
        //Snaps UV to frame, private so that only AnimatedSprite class can call
        void RecalcUV();

    }; //Class AnimatedSprite

    //Instances_________________________________________
    //ADD INSTANCES [HERE] FOR DIFFERENT SPRITES
    extern AnimatedSprite background;
    extern AnimatedSprite player;
    extern AnimatedSprite asteroid;
    extern AnimatedSprite cheese;
    extern AnimatedSprite gameLogo;
    extern AnimatedSprite UIButton;
    // TODO declare animatedsprite
    extern AnimatedSprite spike;
	extern AnimatedSprite wall;
	extern AnimatedSprite cat;


}//Namespace ANIMATION
