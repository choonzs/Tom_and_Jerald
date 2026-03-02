#include "pch.hpp"
#ifndef IMGFONTINIT_HPP
#define IMGFONTINIT_HPP
#pragma once

namespace ASSETS {
	extern AEGfxTexture* playerTexture;
	extern AEGfxTexture* backgroundTexture;
	extern AEGfxTexture* copyrightText;
	extern AEGfxTexture* copyrightLogo;
	extern AEGfxTexture* backgroundAssets; //Background + obstacles
	extern AEGfxTexture* otherAssets;		//Other assets + UI
	void Init_Images();
	void Init_Font();

	void Unload_Images();
	void Unload_Font();

}


#endif // !IMG_FONT_INIT_HPP