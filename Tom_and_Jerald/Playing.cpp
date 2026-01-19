#include "pch.h"
#include "utils.h"
#include "Playing.h"
#include "GameStateList.h"
#include "PlayerMovement.h"
#include "GameStateManager.h"

f32 stage_timer = 0.0f;
f32 damage_timer = 0.0f;

Player base_player = {};
Obstacle obstacles[k_obstacle_count] = {};
namespace {
	s8 font_id;
	AEGfxVertexList* unit_square = nullptr, *unit_circle = nullptr;
}
void resetStage(Player* player, Obstacle* obstacles, f32* stage_timer, f32* damage_timer);
void updateObstacles(Obstacle* obstacles, f32 delta_time);
static void drawHealthBar(AEGfxVertexList* mesh, const Player* player);


void Playing_Load() {
	font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);
	resetStage(&base_player, obstacles, &stage_timer, &damage_timer);
}

void Playing_Initialize() {
	createUnitSquare(&unit_square);
	createUnitCircles(&unit_circle);

	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetCamPosition(0.0f, 0.0f);
}

void Playing_Update() {
	f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();
	stage_timer += delta_time;
	if (damage_timer > 0.0f)
		damage_timer -= delta_time;

	handlePlayerMovement(&base_player, delta_time);
	// Your own rendering logic goes here
	updateObstacles(obstacles, delta_time);

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		if (checkOverlap(&base_player.position, &base_player.half_size, &obstacles[i].position, &obstacles[i].half_size))
		{
			if (damage_timer <= 0.0f)
			{
				base_player.health -= 1;
				damage_timer = k_damage_cooldown;
			}
		}
	}
	// Informing the system about the loop's end
	if (base_player.health <= 0)
		next = GAME_STATE_GAME_OVER;
	else if (stage_timer >= k_stage_duration)
		next = GAME_STATE_VICTORY;
	else {
		// Loop back on same scene
		next = GAME_STATE_RESTART;
	}

}

void Playing_Draw() {
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetTransparency(1.0f);

	drawQuad(unit_circle, base_player.position.x, base_player.position.y, base_player.half_size.x * 2.0f, base_player.half_size.y * 2.0f, 0.25f, 0.7f, 1.0f, 1.0f);

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		Obstacle* obstacle = &obstacles[i];
		drawQuad(unit_square, obstacle->position.x, obstacle->position.y, obstacle->half_size.x * 2.0f, obstacle->half_size.y * 2.0f, 1.0f, 0.4f, 0.35f, 1.0f);
	}
	drawHealthBar(unit_square, &base_player);
	
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	char timer_text[64];
	f32 time_left = k_stage_duration - stage_timer;
	if (time_left < 0.0f) time_left = 0.0f;
	sprintf_s(timer_text, "TIME LEFT: %.1f", time_left);
	AEGfxPrint(font_id, timer_text, -0.95f, 0.85f, 0.45f, 0.9f, 0.9f, 0.9f, 1.0f);
}

void Playing_Free() {
	AEGfxMeshFree(unit_square);
}

void Playing_Unload() {
	AEGfxDestroyFont(font_id);
}

void resetObstacle(Obstacle* obstacle) {
	f32 max_x = AEGfxGetWinMaxX();
	f32 min_y = AEGfxGetWinMinY();
	f32 max_y = AEGfxGetWinMaxY();
	f32 size_value = randomRange(25.0f, 70.0f);
	AEVec2Set(&obstacle->half_size, size_value, size_value);
	AEVec2Set(
		&obstacle->position,
		//move it a little offscreen
		max_x + obstacle->half_size.x,
		randomRange(min_y + size_value, max_y - size_value));
	AEVec2Set(
		&obstacle->velocity,
		randomRange(-220.0f, 220.0f),
		randomRange(-220.0f, 220.0f));
}

void resetStage(Player* player, Obstacle* obstacle, f32* stage_time, f32* damage_time) {
	AEVec2Set(&player->position, 0.0f, 0.0f);
	AEVec2Set(&player->half_size, 25.0f, 25.0f);
	player->health = k_max_health;
	*stage_time = 0.0f;
	*damage_time = 0.0f;

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		resetObstacle(obstacle + i);
	}
}



void updateObstacles(Obstacle* Obstacles, f32 delta_time)
{
	f32 min_x = AEGfxGetWinMinX();
	f32 max_x = AEGfxGetWinMaxX();
	f32 min_y = AEGfxGetWinMinY();
	f32 max_y = AEGfxGetWinMaxY();

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		Obstacle* obstacle = &Obstacles[i];
		obstacle->position.x += obstacle->velocity.x * delta_time;
		obstacle->position.y += obstacle->velocity.y * delta_time;

		// Bounce off walls left boundary
		if (obstacle->position.x - obstacle->half_size.x < min_x - (3 * obstacle->half_size.x))
		{
			// Reset obj positon
			resetObstacle(obstacle);
		}

		/*if (obstacle->position.x - obstacle->half_size.x < min_x)
		{
			obstacle->position.x = min_x + obstacle->half_size.x;
			obstacle->velocity.x *= -1.0f;
		}*/

		// Bounce off walls right boundary
		if (obstacle->position.x + obstacle->half_size.x > max_x + (3 * obstacle->half_size.x))
		{
			obstacle->velocity.x *= -1.0f;
		}

		// Bounce off walls bottom boundary
		if (obstacle->position.y - obstacle->half_size.y < min_y)
		{
			obstacle->position.y = min_y + obstacle->half_size.y;
			obstacle->velocity.y *= -1.0f;
		}

		// Bounce off walls top boundary
		if (obstacle->position.y + obstacle->half_size.y > max_y)
		{
			obstacle->position.y = max_y - obstacle->half_size.y;
			obstacle->velocity.y *= -1.0f;
		}
	}
}

static void drawHealthBar(AEGfxVertexList* mesh, const Player* player)
{
	f32 min_x = AEGfxGetWinMinX();
	f32 max_y = AEGfxGetWinMaxY();
	f32 bar_width = 32.0f;
	f32 bar_height = 12.0f;
	f32 spacing = 6.0f;
	f32 start_x = min_x + 30.0f;
	f32 start_y = max_y - 30.0f;

	for (int i{}; i < k_max_health; ++i)
	{
		f32 center_x = start_x + (bar_width + spacing) * i;
		f32 center_y = start_y;
		drawQuad(mesh, center_x, center_y, bar_width, bar_height, 0.15f, 0.15f, 0.15f, 1.0f);
		if (i < player->health)
			drawQuad(mesh, center_x, center_y, bar_width - 4.0f, bar_height - 4.0f, 0.2f, 0.9f, 0.35f, 1.0f);
	}
}

