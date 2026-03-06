#include "pch.hpp"
#include "Utils.hpp"
#include "Animation.hpp"


namespace ANIMATION {
    class Spritesheet {
    public:
        u32 rows, cols, max_sprites;
        f32 uv_width, uv_height;
    };
    //Create instances of the spritesheet class
    Spritesheet ss_background;
    Spritesheet ss_other;

    class Animation {
    public:
        f32 timer{}, duration_per_frame{ 2.0f };
        u32 active_row{ 0 };
        u32 col_start{ 0 };
        u32 col_count{ 0 };

    };
    //Create instances of the animation class
    Animation animation;
    

    u32 current_sprite_index = 0; // start from first sprite
    f32 current_sprite_uv_offset_x = 0.f;
    f32 current_sprite_uv_offset_y = 0.f;
    
    // Reading Data for this script only
    bool ImportDataFromFile(const char* filename) {
        std::ifstream ifs(filename);

        if (!ifs) { return false; }

        std::string tmp;
        ifs >> tmp;
        ifs >> ss_background.rows;
        ifs >> tmp;
        ifs >> ss_background.cols;
        ifs >> tmp;
        ifs >> ss_background.max_sprites;

        return true;
    }

    void sprite_Initialize() {
        if (!ImportDataFromFile("Assets/AnimationData.txt")) {
            std::cerr << "Unable to open file!" << std::endl;

        }
        ss_background.uv_width = 1.f / ss_background.cols;
        ss_background.uv_height = 1.f / ss_background.rows;

        //backround frames - need to pass these in later
        animation.active_row = 0;
        animation.col_start = 0;
        animation.col_count = 3;  // only play 3 frames
        current_sprite_index = 0;
    }

	void sprite_update(f32 delta_time) {
		animation.timer += delta_time;

        if (animation.timer >= animation.duration_per_frame) {
            // When the time is up go to the next sprite.
            // Reset the timer.
            animation.timer = 0;

            // Calculate the next sprite UV
            /* old code
            current_sprite_index = ++current_sprite_index % ss_background.max_sprites;

            u32 current_sprite_row = current_sprite_index / ss_background.cols;
            u32 current_sprite_col = current_sprite_index % ss_background.cols;

            current_sprite_uv_offset_x = ss_background.uv_width * current_sprite_col;
            current_sprite_uv_offset_y = ss_background.uv_height * current_sprite_row;*/


            u32 total_cols = (animation.col_count > 0) ? animation.col_count : ss_background.cols;

            // current_sprite_index tracks just the column (0 to total_cols-1)
            current_sprite_index = (current_sprite_index + 1) % total_cols;

            current_sprite_uv_offset_x = ss_background.uv_width * static_cast<f32>(animation.col_start + current_sprite_index);
            current_sprite_uv_offset_y = ss_background.uv_height * static_cast<f32>(animation.active_row);

        }
	}

    void set_sprite_texture(AEGfxTexture* Texture) {
        
        // Tell the engine to get ready to draw something with texture.
        AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);

        // Set the the color to multiply to white, so that the sprite can 
        // display the full range of colors (default is black).
        AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
        AEGfxSetColorToAdd(0.f, 0.0f, 0.0f, 0.0f);

        // Set blend mode to AE_GFX_BM_BLEND
        // This will allow transparency.
        AEGfxSetBlendMode(AE_GFX_BM_BLEND);
        AEGfxSetTransparency(1.0f);

        // Set the texture to pTex
        AEGfxTextureSet(Texture, current_sprite_uv_offset_x, current_sprite_uv_offset_y);
    }
}