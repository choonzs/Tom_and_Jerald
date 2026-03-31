#include "pch.hpp"
#ifndef IMGFONTINIT_HPP
#define IMGFONTINIT_HPP
#pragma once

namespace ASSETS {
	extern AEGfxTexture* (copyrightText);		//Copyright assets
	extern AEGfxTexture* (copyrightLogo);
	extern AEGfxTexture* (playerAssets);		//Player + customization assets
	extern AEGfxTexture* (backgroundAssets);	//Background + obstacles assets
	extern AEGfxTexture* (otherAssets);			//Other game assets
	extern AEGfxTexture* (brandAssets);		    //Game title assets
	extern AEGfxTexture* (UIAssets);			//UI assets

	void Init_Images();
	void Init_Font();
	s8& Font();

	void Unload_Images();
	void Unload_Font();

}


#endif // !IMG_FONT_INIT_HPP