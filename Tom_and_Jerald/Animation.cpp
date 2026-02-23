#include "pch.hpp"
#include "Utils.hpp"
#include "Animation.hpp"


namespace ANIMATION {
    struct Spritesheet {
        u32 rows, cols, max_sprites;
        f32 uv_width, uv_height;
    } spritesheet;

    struct Animation {
        f32 timer{}, duration_per_frame{ 0.1f };
    } animation;
    

    u32 current_sprite_index = 0; // start from first sprite
    f32 current_sprite_uv_offset_x = 0.f;
    f32 current_sprite_uv_offset_y = 0.f;
    
    // Reading Data for this script only
    bool ImportDataFromFile(const char* filename) {
        std::ifstream ifs(filename);

        if (!ifs) { return false; }

        std::string tmp;
        ifs >> tmp;
        ifs >> spritesheet.rows;
        ifs >> tmp;
        ifs >> spritesheet.cols;
        ifs >> tmp;
        ifs >> spritesheet.max_sprites;

        return true;
    }

    void sprite_Initialize() {
        if (!ImportDataFromFile("Assets/AnimationData.txt")) {
            std::cerr << "Unable to open file!" << std::endl;

        }
        spritesheet.uv_width = 1.f / spritesheet.cols;
        spritesheet.uv_height = 1.f / spritesheet.rows;
    }

	void sprite_update(f32 delta_time) {
		animation.timer += delta_time;

        if (animation.timer >= animation.duration_per_frame) {
            // When the time is up go to the next sprite.
            // Reset the timer.
            animation.timer = 0;

            // Calculate the next sprite UV
            current_sprite_index = ++current_sprite_index % spritesheet.max_sprites;

            u32 current_sprite_row = current_sprite_index / spritesheet.cols;
            u32 current_sprite_col = current_sprite_index % spritesheet.cols;

            current_sprite_uv_offset_x = spritesheet.uv_width * current_sprite_col;
            current_sprite_uv_offset_y = spritesheet.uv_height * current_sprite_row;

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