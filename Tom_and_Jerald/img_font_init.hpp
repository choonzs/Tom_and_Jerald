#include "pch.hpp"
#ifndef IMG_FONT_INIT_HPP
#define IMG_FONT_INIT_HPP
#pragma once

namespace ASSETS {
	AEGfxTexture* playerTexture;
	AEGfxTexture* backgroundTexture;
	void Init_Images();
	void Init_Font();

	void Unload_Images();
	void Unload_Font();

}


#endif // !IMG_FONT_INIT_HPP