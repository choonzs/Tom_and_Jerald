#include "pch.hpp"
#include "Utils.hpp"
#include "Animation.hpp"


namespace ANIMATION {
    //Instances_________________________________
    //ADD INSTANCES [HERE] FOR DIFFERENT SPRITES
    AnimatedSprite background;
    AnimatedSprite player;
    AnimatedSprite asteroid;
    AnimatedSprite cheese;
    AnimatedSprite gameLogo;
    AnimatedSprite UIButton;
    // TODO define animatedsprite
    AnimatedSprite spike;
    AnimatedSprite wall;
        
    //Member functions__________________________
    //Initialize spritesheet
    void AnimatedSprite::Anim_Init(u32 total_rows, u32 total_cols) {    //Pass in total rows + cols of spritesheet 
        rows = total_rows;                                              //Store the params for other function use
        cols = total_cols;
        max_sprites = rows * cols;
        uv_width = 1.0f / static_cast<f32>(cols);                       //Explicit cast to f32
        uv_height = 1.0f / static_cast<f32>(rows);
        //std::cout << "Anim_Init called: rows=" << rows << " cols=" << cols << "\n";//DEBUG LINE, comment out

        RecalcUV();                                                     //Calculate UV for frame 0, prepares for Anim_Update()
    }

    //Read information from external file and pass to Anim_Init to iniaialize.
    bool AnimatedSprite::ImportFromFile(const char* filename) {         //Pass in .txt file path
        std::ifstream ifs(filename);

        if (!ifs) {                                                     //Safeguard
            std::cerr << "Cannot open " << filename << "\n";
            return false; 
        }

        std::string tmp;                                                //Absorbs text so that numbers can be passed
        u32 total_rows{}, total_cols{}, total_sprites{};                //Holds the passed number
        ifs >> tmp >> total_rows;
        ifs >> tmp >> total_cols;
        ifs >> tmp >> total_sprites;
        //std::cout << "Import called: rows=" << total_rows << " cols=" << total_cols << "\n";//DEBUG LINE, comment out

        Anim_Init(total_rows, total_cols);
        return true;
    }

    //Set up specific animation clip
    void AnimatedSprite::Clip_Select(u32 row, u32 start_col, u32 frame_count, f32 fps) { //Pass in row, col and frames for clip
        active_row = row;                                               //Store data passed to class members
        col_start = start_col;
        col_count = frame_count;
        duration_per_frame = 1.0f / fps;                                //Converts fps into seconds per frame(60fps: 1 sec/60 frames = 0.0166..)
        current_index = 0;                                              //Reset class members so that animation plays from the start
        timer = 0.0f;
        RecalcUV();                                                     //Snap UV to first frame
    }

    //Updates animation clip to run
    void AnimatedSprite::Anim_Update(f32 delta_time) {                  //Pass in time in seconds since last game tick
        timer += delta_time;                                            //Accumilates tick until time to advance a frame
        if (timer >= duration_per_frame) {
            timer -= duration_per_frame;                                //Carry over leftover time, so that animation plays at correct speed
            current_index = (current_index + 1) % col_count;            //For clip to loop
            RecalcUV();                                                 //Snap to frame on frame change
        }
    }
    //Renders the clip
    void AnimatedSprite::Anim_Draw(AEGfxTexture* Texture) const {       //Pass in texture pointer, const to make sure members are not modified
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);                          //Tell engine to get ready to draw texture
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);                //Set color to multiply to white, so sprite can display full range of colors (default is black).
        AEGfxSetColorToAdd(0.f, 0.0f, 0.0f, 0.0f);                      //No additive colour, unless glow effect
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);                             //Enable transparency blend mode for PNGs
        AEGfxSetTransparency(1.0f);
        AEGfxTextureSet(Texture, uv_offset_x, uv_offset_y);             //Set the texture to pTex, passes UV offest from RecalcUV()
   
    }

//Private functions_________________________
    //Snaps UV to frame, private so that only AnimatedSprite class can call
    void AnimatedSprite::RecalcUV() {
        uv_offset_x = uv_width * static_cast<f32>(col_start + current_index);//Selects col + steps to the end of the clip
        uv_offset_y = uv_height * static_cast<f32>(active_row);         //Selects row where clip resides
    }

}//Namespace ANIMATION