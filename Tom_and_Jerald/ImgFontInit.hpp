#include "pch.hpp"
#ifndef IMGFONTINIT_HPP
#define IMGFONTINIT_HPP
#pragma once

namespace ASSETS {
	extern AEGfxTexture* backgroundTexture;
	extern AEGfxTexture* copyrightText;
	extern AEGfxTexture* copyrightLogo;
	extern AEGfxTexture* backgroundAssets; //Background + obstacles
	extern AEGfxTexture* otherAssets;		//Other assets + UI
	extern AEGfxTexture* playerAssets;		//Player + customization
	void Init_Images();
	void Init_Font();
	s8& Font();

	void Unload_Images();
	void Unload_Font();

}


#endif // !IMG_FONT_INIT_HPP