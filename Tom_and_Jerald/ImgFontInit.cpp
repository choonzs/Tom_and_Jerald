#include "pch.hpp"
#include "imgfontinit.hpp"


namespace ASSETS {
	AEGfxTexture* copyrightText = nullptr;		 //Copyright assets
	AEGfxTexture* copyrightLogo = nullptr;
	AEGfxTexture* playerAssets = nullptr;		 //Player + customization assets
	AEGfxTexture* backgroundAssets = nullptr;	 //Background + obstacles assets
	AEGfxTexture* otherAssets = nullptr;		 //Other game assets
	AEGfxTexture* brandAssets = nullptr;		 //Game title assets
	AEGfxTexture* UIAssets = nullptr;			 //UI assets

	s8 font_id;

	void Init_Images() {
		copyrightLogo = AEGfxTextureLoad("Assets/DigipenLogo.png");				//Copyright assets
		copyrightText = AEGfxTextureLoad("Assets/Copyright.png");
		backgroundAssets = AEGfxTextureLoad("Assets/SS_BackgroundAssets.png"); //Background + obstacles assets
		otherAssets = AEGfxTextureLoad("Assets/SS_OtherAssets.png");		   //Other game assets
		playerAssets = AEGfxTextureLoad("Assets/SS_PlayerAssets.png");		   //Player + customization assets
		brandAssets = AEGfxTextureLoad("Assets/SS_BrandAssets.png");		   //Game title assets
		UIAssets = AEGfxTextureLoad("Assets/SS_UIAssets.png");				   //UI assets

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
		AEGfxTextureUnload(UIAssets);


		playerAssets = nullptr;
		copyrightLogo = nullptr;
		copyrightText = nullptr;
		backgroundAssets = nullptr;
		otherAssets = nullptr;
		brandAssets = nullptr;
		UIAssets = nullptr;
	}

	void Unload_Font() {
		AEGfxDestroyFont(font_id);
	}
}