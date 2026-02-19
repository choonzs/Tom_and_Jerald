#include "pch.hpp"
#include "Utils.hpp"
#include "Animation.hpp"

namespace ANIMATION {
    // (Not so good) hardcoded values about our spritesheet
    const u32 spritesheet_rows = 2;
    const u32 spritesheet_cols = 2;
    const u32 spritesheet_max_sprites = 3;
    const f32 sprite_uv_width = 1.f / spritesheet_cols;
    const f32 sprite_uv_height = 1.f / spritesheet_rows;

    // (Not so good) hardcoded values about our animation
    f32 animation_timer = 0.f;
    f32 animation_duration_per_frame = 0.1f;
    u32 current_sprite_index = 0; // start from first sprite
    f32 current_sprite_uv_offset_x = 0.f;
    f32 current_sprite_uv_offset_y = 0.f;

	void sprite_update(f32 delta_time) {
		animation_timer += delta_time;

        if (animation_timer >= animation_duration_per_frame) {
            // When the time is up go to the next sprite.
            // Reset the timer.
            animation_timer = 0;

            // Calculate the next sprite UV
            current_sprite_index = ++current_sprite_index % spritesheet_max_sprites;

            u32 current_sprite_row = current_sprite_index / spritesheet_cols;
            u32 current_sprite_col = current_sprite_index % spritesheet_cols;

            current_sprite_uv_offset_x = sprite_uv_width * current_sprite_col;
            current_sprite_uv_offset_y = sprite_uv_height * current_sprite_row;

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