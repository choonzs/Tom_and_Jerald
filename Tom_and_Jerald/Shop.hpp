#pragma once
#ifndef SHOP_HPP
#define SHOP_HPP

#include "pch.hpp"

class ShopState
{
public:
	void Load();
	void Initialize();
	void Update();
	void Draw();
	void Free();
	void Unload();

private:
	void getCursorNormalized(f32* out_x, f32* out_y) const;
	bool isCursorOverText(const char* text, f32 center_y, f32 scale, f32 cursor_x, f32 cursor_y) const;
	void drawButtonBackground(const char* text, f32 center_y, f32 scale, f32 padding, f32 red, f32 green, f32 blue) const;

private:
	s8 font_id = -1;
	AEGfxVertexList* unit_square = nullptr;
};

void Shop_Load();
void Shop_Initialize();
void Shop_Update();
void Shop_Draw();
void Shop_Free();
void Shop_Unload();

#endif // SHOP_HPP