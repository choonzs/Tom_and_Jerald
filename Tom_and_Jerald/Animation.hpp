#include "pch.hpp"
#ifndef ANIMATION_HPP
#define ANIMATION_HPP

namespace ANIMATION {
	void player_sprite_update(f32 delta_time);
	void set_player_sprite_texture(AEGfxTexture* playerTexture, AEGfxVertexList* playerMesh);

}

#endif // !ANIMATION_HPP