#include "pch.hpp"
#include "imgfontinit.hpp"


namespace ASSETS {
	AEGfxTexture* playerTexture = nullptr;
	AEGfxTexture* backgroundTexture = nullptr;
	AEGfxTexture* copyrightText = nullptr;
	AEGfxTexture* copyrightLogo = nullptr;
	AEGfxTexture* backgroundAssets = nullptr; //Background + obstacles
	AEGfxTexture* otherAssets = nullptr;		 //Other assets + UI

	s8 font_id;

	void Init_Images() {
		playerTexture = AEGfxTextureLoad("Assets/Fairy_Rat.png");
		copyrightLogo = AEGfxTextureLoad("Assets/DigipenLogo.png");
		copyrightText = AEGfxTextureLoad("Assets/Copyright.png");
		backgroundAssets = AEGfxTextureLoad("Assets/SS_BackgroundAssets.png"); //Background + obstacles
		otherAssets = AEGfxTextureLoad("Assets/SS_OtherAssets.png");		   //Other assets + UI

	}
	void Init_Font() {
		font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	}
	s8& Font() {return font_id;}

	void Unload_Images() {
		AEGfxTextureUnload(playerTexture);
		AEGfxTextureUnload(copyrightLogo);
		AEGfxTextureUnload(copyrightText);
		AEGfxTextureUnload(backgroundAssets);
		AEGfxTextureUnload(otherAssets);
	}

	void Unload_Font() {
		AEGfxDestroyFont(font_id);
	}
}