#include "pch.hpp"
#include "imgfontinit.hpp"


namespace ASSETS {
	AEGfxTexture* playerAssets = nullptr;
	AEGfxTexture* backgroundTexture = nullptr;
	AEGfxTexture* copyrightText = nullptr;
	AEGfxTexture* copyrightLogo = nullptr;
	AEGfxTexture* backgroundAssets = nullptr;	 //Background + obstacles
	AEGfxTexture* otherAssets = nullptr;		 //Other assets + UI
	AEGfxTexture* brandAssets = nullptr;

	s8 font_id;

	void Init_Images() {
		copyrightLogo = AEGfxTextureLoad("Assets/DigipenLogo.png");
		copyrightText = AEGfxTextureLoad("Assets/Copyright.png");
		backgroundAssets = AEGfxTextureLoad("Assets/SS_BackgroundAssets.png"); //Background + obstacles
		otherAssets = AEGfxTextureLoad("Assets/SS_OtherAssets.png");		   //Other assets + UI
		playerAssets = AEGfxTextureLoad("Assets/SS_PlayerAssets.png");		   //Player + customization
		brandAssets = AEGfxTextureLoad("Assets//SS_BrandAssets.png");		   //Game name

	}

	void Init_Font() {
		font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	}
	s8& Font() {return font_id;}

	void Unload_Images() {
		AEGfxTextureUnload(playerAssets);
		AEGfxTextureUnload(copyrightLogo);
		AEGfxTextureUnload(copyrightText);
		AEGfxTextureUnload(backgroundAssets);
		AEGfxTextureUnload(otherAssets);
		AEGfxTextureUnload(brandAssets);

		playerAssets = nullptr;
		copyrightLogo = nullptr;
		copyrightText = nullptr;
		backgroundAssets = nullptr;
		otherAssets = nullptr;
		brandAssets = nullptr;
	}

	void Unload_Font() {
		AEGfxDestroyFont(font_id);
	}
}