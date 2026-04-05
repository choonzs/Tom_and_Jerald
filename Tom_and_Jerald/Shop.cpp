#include "pch.hpp"
#include "Shop.hpp"
#include "Credits.hpp"
#include "Upgrades.hpp"
#include "Utils.hpp"
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "Audio.hpp"
#include "ImgFontInit.hpp"

namespace {
	const f32 k_title_y = 0.75f;
	const f32 k_credits_y = 0.60f;
	const f32 k_button_scale = 0.65f;
	const f32 k_footer_y = -0.75f;
	const int k_upgrade_cost = 10;

	// Button Y Positions
	const f32 k_health_button_y = 0.35f;
	const f32 k_size_button_y = 0.15f;
	const f32 k_fuel_cap_button_y = -0.05f;
	const f32 k_fuel_spawn_button_y = -0.25f;
	const f32 k_fuel_restore_button_y = -0.45f;

	ShopState g_shop;
}

void ShopState::getCursorNormalized(f32* out_x, f32* out_y) const {
	s32 cursor_x = 0;
	s32 cursor_y = 0;
	AEInputGetCursorPosition(&cursor_x, &cursor_y);
	f32 width = (f32)AEGfxGetWindowWidth();
	f32 height = (f32)AEGfxGetWindowHeight();
	*out_x = (cursor_x / width) * 2.0f - 1.0f;
	*out_y = 1.0f - (cursor_y / height) * 2.0f;
}

bool ShopState::isCursorOverText(const char* text, f32 center_y, f32 scale, f32 cursor_x, f32 cursor_y) const {
	f32 width = 0.0f;
	f32 height = 0.0f;
	AEGfxGetPrintSize(font_id, text, scale, &width, &height);
	f32 padding = 0.08f; // match the padding used in drawButtonBackground

	f32 left = -(width + padding) * 0.5f;
	f32 right = (width + padding) * 0.5f;
	f32 bottom = center_y - padding * 0.5f;           // centered, not bottom-anchored
	f32 top = center_y + height + padding * 0.5f;
	return cursor_x >= left && cursor_x <= right && cursor_y >= bottom && cursor_y <= top;
}

void ShopState::drawButtonBackground(const char* text, f32 center_y, f32 scale, f32 padding, f32 red, f32 green, f32 blue) const {
	f32 width = 0.0f;
	f32 height = 0.0f;
	AEGfxGetPrintSize(font_id, text, scale, &width, &height);
	f32 world_width = AEGfxGetWinMaxX() - AEGfxGetWinMinX();
	f32 world_height = AEGfxGetWinMaxY() - AEGfxGetWinMinY();
	f32 center_x_world = 0.0f;
	f32 center_y_world = (center_y + height * 0.5f) * (world_height * 0.5f);
	f32 button_width_world = (width + padding) * (world_width * 0.5f);
	f32 button_height_world = (height + padding) * (world_height * 0.5f);
	drawQuad(unit_square, center_x_world, center_y_world, button_width_world, button_height_world, red, green, blue, 0.35f);
}

void ShopState::Load() {
	ASSETS::Init_Font();
	font_id = ASSETS::Font();
}

void ShopState::Initialize() {
	createUnitSquare(&unit_square);
	Credits_LoadFile("Assets/data/Cheese.txt");
	
	AEGfxSetCamPosition(0.0f, 0.0f);
	AEInputShowCursor(true);

	Upgrades_ReadFromFile("Assets/data/Upgrades.txt");
}

void ShopState::Update() {

	if (AEInputCheckTriggered(AEVK_ESCAPE)) {
		next = previous;
		Credits_SaveFile("Assets/data/Cheese.txt");
		return;
	}

	f32 cx = 0.0f, cy = 0.0f;
	getCursorNormalized(&cx, &cy);

	char hp_text[128], size_text[128], cap_text[128], spawn_text[128], restore_text[128];
	sprintf_s(hp_text, "UPGRADE HEALTH (+5%%) [LEVEL %d/%d]", Upgrades_GetHealthLevel(), k_health_upgrade_max_level);
	sprintf_s(size_text, "UPGRADE SIZE (-0.1) [LEVEL %d/%d]", Upgrades_GetSizeLevel(), k_size_upgrade_max_level);
	sprintf_s(cap_text, "UPGRADE FUEL CAP (+10%%) [LEVEL %d/%d]", Upgrades_GetFuelCapLevel(), k_fuel_upgrade_max_level);
	sprintf_s(spawn_text, "UPGRADE FUEL SPAWN (+5%%) [LEVEL %d/%d]", Upgrades_GetFuelSpawnLevel(), k_fuel_upgrade_max_level);
	sprintf_s(restore_text, "UPGRADE FUEL RECOVERY (+5%%) [LEVEL %d/%d]", Upgrades_GetFuelRestoreLevel(), k_fuel_upgrade_max_level);

	if (AEInputCheckTriggered(AEVK_LBUTTON)) {
		if (isCursorOverText(hp_text, k_health_button_y, k_button_scale, cx, cy) && Upgrades_CanUpgradeHealth() && Credits_Spend(k_upgrade_cost)) {
			Upgrades_UpgradeHealth();
			Upgrades_WriteToFile("Assets/data/Upgrades.txt");
		}
		else if (isCursorOverText(size_text, k_size_button_y, k_button_scale, cx, cy) && Upgrades_CanUpgradeSize() && Credits_Spend(k_upgrade_cost)) {
			Upgrades_UpgradeSize();
			Upgrades_WriteToFile("Assets/data/Upgrades.txt");
		}
		else if (isCursorOverText(cap_text, k_fuel_cap_button_y, k_button_scale, cx, cy) && Upgrades_CanUpgradeFuelCap() && Credits_Spend(k_upgrade_cost)) {
			Upgrades_UpgradeFuelCap();
			Upgrades_WriteToFile("Assets/data/Upgrades.txt");
		}
		else if (isCursorOverText(spawn_text, k_fuel_spawn_button_y, k_button_scale, cx, cy) && Upgrades_CanUpgradeFuelSpawn() && Credits_Spend(k_upgrade_cost)) {
			Upgrades_UpgradeFuelSpawn();
			Upgrades_WriteToFile("Assets/data/Upgrades.txt");
		}
		else if (isCursorOverText(restore_text, k_fuel_restore_button_y, k_button_scale, cx, cy) && Upgrades_CanUpgradeFuelRestore() && Credits_Spend(k_upgrade_cost)) {
			Upgrades_UpgradeFuelRestore();
			Upgrades_WriteToFile("Assets/data/Upgrades.txt");
		}
	}
}

void ShopState::Draw() {
	// Clear the screen & Set to Default
	AEGfxSetBackgroundColor(0.0f, 0.0f, 0.0f);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetColorToMultiply(1.0f, 1.0f, 1.0f, 1.0f);
	AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetTransparency(1.0f);
	AEGfxTextureSet(nullptr, 0, 0);
	/******************************************************************************/

	drawCenteredText(font_id, "SHOP", k_title_y, 1.5f);

	char credits_text[64];
	sprintf_s(credits_text, "CHEESE: %d", Credits_GetBalance());
	drawCenteredText(font_id, credits_text, k_credits_y, 1.0f);

	char hp_text[128], size_text[128], cap_text[128], spawn_text[128], restore_text[128];
	sprintf_s(hp_text, "UPGRADE HEALTH (+5%%) [LEVEL %d/%d]", Upgrades_GetHealthLevel(), k_health_upgrade_max_level);
	sprintf_s(size_text, "UPGRADE SIZE (-0.1) [LEVEL %d/%d]", Upgrades_GetSizeLevel(), k_size_upgrade_max_level);
	sprintf_s(cap_text, "UPGRADE FUEL CAP (+10%%) [LEVEL %d/%d]", Upgrades_GetFuelCapLevel(), k_fuel_upgrade_max_level);
	sprintf_s(spawn_text, "UPGRADE FUEL SPAWN (+5%%) [LEVEL %d/%d]", Upgrades_GetFuelSpawnLevel(), k_fuel_upgrade_max_level);
	sprintf_s(restore_text, "UPGRADE FUEL RECOVERY (+5%%) [LEVEL %d/%d]", Upgrades_GetFuelRestoreLevel(), k_fuel_upgrade_max_level);

	auto DrawBtn = [&](const char* text, f32 y) {
		drawButtonBackground(text, y, k_button_scale, 0.08f, 0.2f, 0.35f, 0.55f);
		drawCenteredText(font_id, text, y, k_button_scale);
		};

	DrawBtn(hp_text, k_health_button_y);
	DrawBtn(size_text, k_size_button_y);
	DrawBtn(cap_text, k_fuel_cap_button_y);
	DrawBtn(spawn_text, k_fuel_spawn_button_y);
	DrawBtn(restore_text, k_fuel_restore_button_y);

	char cost_text[64];
	sprintf_s(cost_text, "COST: %d CHEESE", k_upgrade_cost);
	drawCenteredText(font_id, cost_text, k_fuel_restore_button_y - 0.12f, 0.8f);

	drawCenteredText(font_id, "CLICK AN UPGRADE TO BUY", k_footer_y, 0.8f);
	drawCenteredText(font_id, "RETURN (ESC)", k_footer_y - 0.1f, 0.f);
}

void ShopState::Free() {
	AEGfxMeshFree(unit_square);
	unit_square = nullptr;
	AEInputShowCursor(0);
}

void ShopState::Unload() {
	ASSETS::Unload_Font();
	font_id = -1;
}

// Global wrapper functions
void Shop_Load() { g_shop.Load(); }
void Shop_Initialize() { g_shop.Initialize(); }
void Shop_Update() { g_shop.Update(); }
void Shop_Draw() { g_shop.Draw(); }
void Shop_Free() { g_shop.Free(); }
void Shop_Unload() { g_shop.Unload(); }


