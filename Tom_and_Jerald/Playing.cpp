#include "pch.hpp"
#include "utils.hpp"
#include "Playing.hpp"
#include "Obstacle.hpp"
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "Credits.hpp"
#include "Upgrades.hpp"
#include "Camera.hpp"
#include "Animation.hpp"
#include "JetpackFuel.hpp"
#include "Particles.hpp"
#include "Audio.hpp"

#include "Player.hpp"

f32 stage_timer = 0.0f;
f32 damage_timer = 0.0f;
JetpackFuel* pFuel = nullptr;
Obstacle obstacles[k_obstacle_count] = {};
Player base_player;
AEGfxTexture* background_texture = nullptr;
AEGfxTexture* asteroid_texture = nullptr;
AEGfxTexture* fuel_pickup_texture = nullptr;

namespace {
	Camera camera;
	s8 font_id;
	AEGfxVertexList* unit_square = nullptr;
	AEGfxVertexList* unit_circle = nullptr;

	// Fuel Pickup Logic
	struct FuelPickup {
		AEVec2 pos;
		bool active = false;
		f32 size = 30.0f;
	} g_fuel_pickup;
}

void resetStage(Player& player, Obstacle* obstacles, f32* stage_timer, f32* damage_timer);
int getMaxHealthFromUpgrades();

void Playing_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	base_player.Texture() = AEGfxTextureLoad("Assets/Fairy_Rat.png");
	background_texture = AEGfxTextureLoad("Assets/Game_Background.png");
	asteroid_texture = AEGfxTextureLoad("Assets/PlanetTexture.png");

	// Attempt to load fuel pickup texture (will just draw a colored box if missing)
	fuel_pickup_texture = AEGfxTextureLoad("Assets/FuelPickup.png");

	resetStage(base_player, obstacles, &stage_timer, &damage_timer);
}

void Playing_Initialize() {
	createUnitSquare(&unit_square, .5f, .5f);
	createUnitCircles(&unit_circle);
	createUnitSquare(&(base_player.Mesh()), .5f, .5f);

	// Apply MAX FUEL Upgrade logic when creating JetpackFuel
	f32 base_capacity = 100.0f;
	f32 upgraded_capacity = base_capacity * Upgrades_GetMaxFuelMultiplier();
	pFuel = new JetpackFuel(upgraded_capacity, 30.0f, 2.0f);

	g_fuel_pickup.active = false;

	ANIMATION::sprite_Initialize();
	camera.Magnitude() = 20.0f;
	AEGfxSetCamPosition(camera.Position().x, camera.Position().y);
}

void Playing_Update() {
	f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
	bool isFlying = AEInputCheckCurr(AEVK_SPACE) && pFuel->HasFuel();

	if (pFuel) {
		pFuel->Update(delta_time, isFlying);
	}
	ANIMATION::sprite_update(delta_time);

	camera.Update();
	stage_timer += delta_time;

	if (damage_timer > 0.0f)
		damage_timer -= delta_time;

	base_player.Movement(delta_time);
	updateObstacles(obstacles, delta_time);

	// --- FUEL PICKUP SPAWNING & COLLISION ---
	if (pFuel) {
		f32 fuel_ratio = pFuel->GetCurrentFuel() / pFuel->GetMaxFuel();
		f32 spawn_threshold = 0.50f + Upgrades_GetFuelSpawnBonus();

		// Spawn logic: If active fuel is low enough, spawn off-screen to the RIGHT
		if (!g_fuel_pickup.active && fuel_ratio < spawn_threshold) {
			// Start just outside the right edge of the screen
			g_fuel_pickup.pos.x = AEGfxGetWinMaxX() + 100.0f;

			// Random Y between top and bottom screen bounds
			f32 minY = AEGfxGetWinMinY() + 50.0f;
			f32 maxY = AEGfxGetWinMaxY() - 50.0f;
			g_fuel_pickup.pos.y = minY + (maxY - minY) * ((f32)rand() / RAND_MAX);

			g_fuel_pickup.active = true;
		}

		// Movement, Collision Check & Collection
		if (g_fuel_pickup.active) {
			// FIX: Make the pickup move to the left! 
			// (You can adjust the 350.0f to match how fast your asteroids move)
			g_fuel_pickup.pos.x -= 350.0f * delta_time;

			AEVec2 p_half = { g_fuel_pickup.size / 2.0f, g_fuel_pickup.size / 2.0f };
			if (checkOverlap(&base_player.Position(), &base_player.Half_Size(), &g_fuel_pickup.pos, &p_half)) {
				// Collect pickup, restore fuel
				f32 restore_amt = pFuel->GetMaxFuel() * Upgrades_GetFuelRestorePercentage();
				pFuel->RestoreFuel(restore_amt);
				g_fuel_pickup.active = false;
			}

			// FIX: Despawn if it falls off the LEFT side of the screen
			if (g_fuel_pickup.pos.x < AEGfxGetWinMinX() - 100.0f) {
				g_fuel_pickup.active = false;
			}
		}
	}

	// --- OBSTACLE COLLISION ---
	bool took_damage = false;
	for (int i = 0; i < k_obstacle_count; ++i)
	{
		if (checkOverlap(&(base_player.Position()), &(base_player.Half_Size()), &obstacles[i].position, &obstacles[i].half_size))
		{
			if (damage_timer <= 0.0f)
			{
				PlayRatSqueak();
				base_player.Health() -= 1;
				damage_timer = k_damage_cooldown;
				took_damage = true;
			}
		}
	}
	if (took_damage)
	{
		camera.Set_Shaking();
		Credits_OnDamage();
		graphics::particleInit(base_player.Position().x, base_player.Position().y, 25);
	}

	bool game_active = (base_player.Health() > 0) && (stage_timer < k_stage_duration);
	Credits_Update(delta_time, game_active);

	// State Transitions
	if (base_player.Health() <= 0) next = GAME_STATE_GAME_OVER;
	else if (stage_timer >= k_stage_duration) next = GAME_STATE_VICTORY;
	else if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) next = GAME_STATE_QUIT;
}

void Playing_Draw() {
	// Background
	AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
	ANIMATION::set_sprite_texture(background_texture);
	drawQuad(unit_square, camera.Position().x, camera.Position().y, (AEGfxGetWinMaxX() - AEGfxGetWinMinX()), (AEGfxGetWinMaxY() - AEGfxGetWinMinY()), 1.f, 1.f, 1.f, 1.f);

	// Player
	if (damage_timer <= 0.0f || (int)(damage_timer * 10) % 2 == 0) {
		ANIMATION::set_sprite_texture(base_player.Texture());
		drawQuad(base_player.Mesh(), base_player.Position().x, base_player.Position().y, base_player.Half_Size().x * 2.0f, base_player.Half_Size().y * 2.0f, 1.f, 1.f, 1.f, 1.f);
	}

	// Obstacles
	ANIMATION::set_sprite_texture(asteroid_texture);
	for (int i = 0; i < k_obstacle_count; ++i)
	{
		Obstacle* obstacle = &obstacles[i];
		drawQuad(unit_square, obstacle->position.x, obstacle->position.y, obstacle->half_size.x * 2.0f, obstacle->half_size.y * 2.0f, 1.0f, 0.4f, 0.35f, 1.0f);
	}

	// Fuel Pickup Render
	if (g_fuel_pickup.active) {
		if (fuel_pickup_texture) {
			AEGfxSetRenderMode(AE_GFX_RM_TEXTURE);
			ANIMATION::set_sprite_texture(fuel_pickup_texture);
			drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			// Fallback yellow box if no texture found
			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			drawQuad(unit_square, g_fuel_pickup.pos.x, g_fuel_pickup.pos.y, g_fuel_pickup.size, g_fuel_pickup.size, 1.0f, 0.9f, 0.0f, 1.0f);
		}
	}

	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxSetColorToAdd(0.0f, 0.0f, 0.0f, 0.0f);

	// UI
	drawHealthBar(unit_square, base_player, getMaxHealthFromUpgrades());

	char cheese_text[64];
	sprintf_s(cheese_text, "Cheese: %d", Credits_GetBalance());
	AEGfxPrint(font_id, cheese_text, -0.95f, 0.75f, 0.4f, 0.9f, 0.9f, 0.2f, 1.0f);

	char fps_text[64];
	sprintf_s(fps_text, "%.1f FPS", 1 / (f32)AEFrameRateControllerGetFrameTime());
	AEGfxPrint(font_id, fps_text, -0.95f, 0.65f, 0.4f, .0f, .0f, .0f, 1.0f);

	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	char timer_text[64];
	f32 time_left = k_stage_duration - stage_timer;
	if (time_left < 0.0f) time_left = 0.0f;
	sprintf_s(timer_text, "TIME LEFT: %.1f", time_left);
	AEGfxPrint(font_id, timer_text, -0.95f, 0.85f, 0.45f, 0.9f, 0.9f, 0.9f, 1.0f);

	// Jetpack fuel gauge
	if (pFuel) {
		float screenX = base_player.Position().x - camera.Position().x;
		float screenY = base_player.Position().y - camera.Position().y;
		float verticalOffset = base_player.Half_Size().y + 20.0f;
		pFuel->Draw(screenX, screenY + verticalOffset, 50.0f, 8.0f);
	}

	graphics::particleDraw(unit_circle);
}

void Playing_Free() {
	AEGfxMeshFree(unit_square);
	AEGfxMeshFree(unit_circle);
	if (pFuel) {
		delete pFuel;
		pFuel = nullptr;
	}
}

void Playing_Unload() {
	AEGfxDestroyFont(font_id);
	AEGfxTextureUnload(background_texture);
	if (fuel_pickup_texture) AEGfxTextureUnload(fuel_pickup_texture);
}

void resetStage(Player& player, Obstacle* obstacle, f32* stage_time, f32* damage_time) {
	AEVec2Set(&(player.Position()), 0.0f, 0.0f);
	f32 size_reduction = Upgrades_GetSizeReduction();
	f32 upgraded_half_size = player.Half_Size().x - size_reduction;
	if (upgraded_half_size < 1.0f)
		upgraded_half_size = 1.0f;
	AEVec2Set(&(player.Half_Size()), upgraded_half_size, upgraded_half_size);
	player.Health() = getMaxHealthFromUpgrades();
	*stage_time = 0.0f;
	*damage_time = 0.0f;
	Credits_ResetRound();

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		resetObstacle(obstacle + i);
	}
}

int getMaxHealthFromUpgrades()
{
	f32 increase = Upgrades_GetHealthIncrease();
	f32 multiplier = 1.0f + increase;
	int max_health = (int)floorf(base_player.Config().MaxHealth() * multiplier);
	if (max_health < 1)
		max_health = 1;
	return max_health;
}