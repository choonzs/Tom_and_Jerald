#include "pch.hpp"
#include "img_font_init.hpp"


namespace ASSETS {
	s8 font_id;
	void Init_Images() {
		playerTexture = AEGfxTextureLoad("Assets/Fairy_Rat.png");
	}
	void Init_Font() {
		font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	}

	void Unload_Images() {
		AEGfxTextureUnload(playerTexture);
	}

	void Unload_Font() {
		AEGfxDestroyFont(font_id);
	}
}