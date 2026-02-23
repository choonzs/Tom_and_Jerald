#include "pch.hpp"
#ifndef ANIMATION_HPP
#define ANIMATION_HPP

namespace ANIMATION {
	void sprite_Initialize();
	void sprite_update(f32 delta_time);
	void set_sprite_texture(AEGfxTexture* Texture);

}

#endif // !ANIMATION_HPP