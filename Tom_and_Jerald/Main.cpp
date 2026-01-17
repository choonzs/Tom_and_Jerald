// ---------------------------------------------------------------------------
// includes test test 123 32323
// main

#include <crtdbg.h> // To check for memory leaks
#include <math.h>
#include <time.h>
#include "AEEngine.h"

struct Obstacle
{
	AEVec2 position;
	AEVec2 velocity;
	AEVec2 half_size;
};

struct Player
{
	AEVec2 position;
	AEVec2 half_size;
	int health;
};

// ---------------------------------------------------------------------------
// main
enum GameState
{
	GAME_STATE_MENU = 0,
	GAME_STATE_PLAYING,
	GAME_STATE_GAME_OVER,
	GAME_STATE_VICTORY
};

static const int k_max_health = 10;
static const int k_obstacle_count = 10;
static const f32 k_stage_duration = 30.0f;
static const f32 k_player_speed = 500.0f;
static const f32 k_damage_cooldown = 0.4f;

static f32 randomRange(f32 min_value, f32 max_value)
{
	return min_value + (max_value - min_value) * (rand() / (f32)RAND_MAX);
}

static void createUnitSquare(AEGfxVertexList** out_mesh)
{
	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
		0.5f, -0.5f, 0xFFFFFFFF, 1.0f, 0.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 1.0f);
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, 0.0f,
		0.5f, 0.5f, 0xFFFFFFFF, 1.0f, 1.0f,
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 1.0f);
	*out_mesh = AEGfxMeshEnd();
}

static void drawQuad(AEGfxVertexList* mesh, f32 center_x, f32 center_y, f32 width, f32 height, f32 red, f32 green, f32 blue, f32 alpha)
{
	AEMtx33 scale;
	AEMtx33 translate;
	AEMtx33 transform;

	AEMtx33Scale(&scale, width, height);
	AEMtx33Trans(&translate, center_x, center_y);
	AEMtx33Concat(&transform, &translate, &scale);

	AEGfxSetTransform(transform.m);
	AEGfxSetColorToMultiply(red, green, blue, alpha);
	AEGfxMeshDraw(mesh, AE_GFX_MDM_TRIANGLES);
}

static bool checkOverlap(const AEVec2* position_a, const AEVec2* half_size_a, const AEVec2* position_b, const AEVec2* half_size_b)
{
	if (fabsf(position_a->x - position_b->x) > (half_size_a->x + half_size_b->x))
		return false;

	if (fabsf(position_a->y - position_b->y) > (half_size_a->y + half_size_b->y))
		return false;

	return true;
}

static void resetStage(Player* player, Obstacle* obstacles, f32* stage_timer, f32* damage_timer)
{
	f32 min_x = AEGfxGetWinMinX();
	f32 max_x = AEGfxGetWinMaxX();
	f32 min_y = AEGfxGetWinMinY();
	f32 max_y = AEGfxGetWinMaxY();

	AEVec2Set(&player->position, 0.0f, 0.0f);
	AEVec2Set(&player->half_size, 25.0f, 25.0f);
	player->health = k_max_health;
	*stage_timer = 0.0f;
	*damage_timer = 0.0f;

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		f32 size_value = randomRange(25.0f, 70.0f);
		AEVec2Set(&obstacles[i].half_size, size_value, size_value);
		AEVec2Set(
			&obstacles[i].position,
			randomRange(min_x + size_value, max_x - size_value),
			randomRange(min_y + size_value, max_y - size_value));
		AEVec2Set(
			&obstacles[i].velocity,
			randomRange(-220.0f, 220.0f),
			randomRange(-220.0f, 220.0f));
	}
}

static void handlePlayerMovement(Player* player, f32 delta_time)
{
	f32 move_x = 0.0f;
	f32 move_y = 0.0f;

	if (AEInputCheckCurr(AEVK_W) || AEInputCheckCurr(AEVK_UP))
		move_y += 1.0f;
	if (AEInputCheckCurr(AEVK_S) || AEInputCheckCurr(AEVK_DOWN))
		move_y -= 1.0f;
	if (AEInputCheckCurr(AEVK_A) || AEInputCheckCurr(AEVK_LEFT))
		move_x -= 1.0f;
	if (AEInputCheckCurr(AEVK_D) || AEInputCheckCurr(AEVK_RIGHT))
		move_x += 1.0f;

	player->position.x += move_x * k_player_speed * delta_time;
	player->position.y += move_y * k_player_speed * delta_time;

	f32 min_x = AEGfxGetWinMinX() + player->half_size.x;
	f32 max_x = AEGfxGetWinMaxX() - player->half_size.x;
	f32 min_y = AEGfxGetWinMinY() + player->half_size.y;
	f32 max_y = AEGfxGetWinMaxY() - player->half_size.y;

	if (player->position.x < min_x)
		player->position.x = min_x;
	if (player->position.x > max_x)
		player->position.x = max_x;
	if (player->position.y < min_y)
		player->position.y = min_y;
	if (player->position.y > max_y)
		player->position.y = max_y;
}

static void updateObstacles(Obstacle* obstacles, f32 delta_time)
{
	f32 min_x = AEGfxGetWinMinX();
	f32 max_x = AEGfxGetWinMaxX();
	f32 min_y = AEGfxGetWinMinY();
	f32 max_y = AEGfxGetWinMaxY();

	for (int i = 0; i < k_obstacle_count; ++i)
	{
		Obstacle* obstacle = &obstacles[i];
		obstacle->position.x += obstacle->velocity.x * delta_time;
		obstacle->position.y += obstacle->velocity.y * delta_time;

		if (obstacle->position.x - obstacle->half_size.x < min_x)
		{
			obstacle->position.x = min_x + obstacle->half_size.x;
			obstacle->velocity.x *= -1.0f;
		}

		if (obstacle->position.x + obstacle->half_size.x > max_x)
		{
			obstacle->position.x = max_x - obstacle->half_size.x;
			obstacle->velocity.x *= -1.0f;
		}

		if (obstacle->position.y - obstacle->half_size.y < min_y)
		{
			obstacle->position.y = min_y + obstacle->half_size.y;
			obstacle->velocity.y *= -1.0f;
		}

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

	for (int i = 0; i < k_max_health; ++i)
	{
		f32 center_x = start_x + (bar_width + spacing) * i;
		f32 center_y = start_y;
		drawQuad(mesh, center_x, center_y, bar_width, bar_height, 0.15f, 0.15f, 0.15f, 1.0f);
		if (i < player->health)
			drawQuad(mesh, center_x, center_y, bar_width - 4.0f, bar_height - 4.0f, 0.2f, 0.9f, 0.35f, 1.0f);
	}
}

static void drawCenteredText(s8 font_id, const char* text, f32 y, f32 scale)
{
	f32 width = 0.0f;
	f32 height = 0.0f;
	AEGfxGetPrintSize(font_id, text, scale, &width, &height);
	AEGfxPrint(font_id, text, -width * 0.5f, y, scale, 1.0f, 1.0f, 1.0f, 1.0f);
}

static void drawMenu(s8 font_id)
{
	drawCenteredText(font_id, "TOM AND JERALD", 0.4f, 1.1f);
	drawCenteredText(font_id, "START (ENTER)", 0.1f, 0.7f);
	drawCenteredText(font_id, "EXIT (ESC)", -0.05f, 0.7f);
	drawCenteredText(font_id, "MOVE: WASD / ARROWS", -0.25f, 0.45f);
	drawCenteredText(font_id, "AVOID THE OBSTACLES FOR 30 SECONDS", -0.35f, 0.45f);
}

static void drawGameOver(s8 font_id)
{
	drawCenteredText(font_id, "GAME OVER", 0.35f, 1.0f);
	drawCenteredText(font_id, "TRY AGAIN (1)", 0.05f, 0.7f);
	drawCenteredText(font_id, "RETURN TO MAIN MENU (2)", -0.1f, 0.7f);
	drawCenteredText(font_id, "EXIT (3)", -0.25f, 0.7f);
}

static void drawVictory(s8 font_id)
{
	drawCenteredText(font_id, "CONGRATULATIONS!", 0.35f, 1.0f);
	drawCenteredText(font_id, "PROCEED TO NEXT STAGE (N)", 0.05f, 0.7f);
	drawCenteredText(font_id, "SAVE AND RETURN TO MAIN MENU (M)", -0.1f, 0.7f);
	drawCenteredText(font_id, "EXIT (ESC)", -0.25f, 0.7f);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand((unsigned int)time(NULL));

	// Initialization of your own variables go here
	int game_running = 1;

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AESysSetWindowTitle("Tom and Jerald - Prototype");
	AESysReset();

	AEGfxSetBackgroundColor(0.06f, 0.07f, 0.09f);
	AEGfxSetBlendMode(AE_GFX_BM_BLEND);
	AEGfxSetRenderMode(AE_GFX_RM_COLOR);
	AEGfxTextureSet(NULL, 0.0f, 0.0f);
	AEGfxSetCamPosition(0.0f, 0.0f);

	AEGfxVertexList* unit_square = NULL;
	createUnitSquare(&unit_square);

	s8 font_id = AEGfxCreateFont("Assets/liberation-mono.ttf", 32);

	Player player = {};
	Obstacle obstacles[k_obstacle_count] = {};
	GameState game_state = GAME_STATE_MENU;
	f32 stage_timer = 0.0f;
	f32 damage_timer = 0.0f;

	resetStage(&player, obstacles, &stage_timer, &damage_timer);

	// Game Loop
		while (game_running)
		{
			// Informing the system about the loop's start
			AESysFrameStart();

			// Basic way to trigger exiting the application
			// when ESCAPE is hit or when the window is closed
			if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist())
			game_running = 0;

			// Your own update logic goes here
			f32 delta_time = (f32)AEFrameRateControllerGetFrameTime();

			if (game_state == GAME_STATE_MENU)
			{
				if (AEInputCheckTriggered(AEVK_RETURN))
				{
					resetStage(&player, obstacles, &stage_timer, &damage_timer);
					game_state = GAME_STATE_PLAYING;
				}
			}
			else if (game_state == GAME_STATE_PLAYING)
			{
				stage_timer += delta_time;
				if (damage_timer > 0.0f)
					damage_timer -= delta_time;

				// Your own rendering logic goes here
				handlePlayerMovement(&player, delta_time);
				updateObstacles(obstacles, delta_time);

				for (int i = 0; i < k_obstacle_count; ++i)
				{
					if (checkOverlap(&player.position, &player.half_size, &obstacles[i].position, &obstacles[i].half_size))
					{
						if (damage_timer <= 0.0f)
						{
							player.health -= 1;
							damage_timer = k_damage_cooldown;
						}
					}
				}

				// Informing the system about the loop's end
				if (player.health <= 0)
					game_state = GAME_STATE_GAME_OVER;
				else if (stage_timer >= k_stage_duration)
					game_state = GAME_STATE_VICTORY;
			}
			else if (game_state == GAME_STATE_GAME_OVER)
			{
				if (AEInputCheckTriggered(AEVK_1))
				{
					resetStage(&player, obstacles, &stage_timer, &damage_timer);
					game_state = GAME_STATE_PLAYING;
				}
				else if (AEInputCheckTriggered(AEVK_2))
				{
					game_state = GAME_STATE_MENU;
				}
				else if (AEInputCheckTriggered(AEVK_3))
				{
					game_running = 0;
				}
			}
			else if (game_state == GAME_STATE_VICTORY)
			{
				if (AEInputCheckTriggered(AEVK_N))
				{
					resetStage(&player, obstacles, &stage_timer, &damage_timer);
					game_state = GAME_STATE_PLAYING;
				}
				else if (AEInputCheckTriggered(AEVK_M))
				{
					game_state = GAME_STATE_MENU;
				}
			}

			AEGfxSetRenderMode(AE_GFX_RM_COLOR);
			AEGfxTextureSet(NULL, 0.0f, 0.0f);
			AEGfxSetTransparency(1.0f);

			if (game_state == GAME_STATE_PLAYING)
			{
				drawQuad(unit_square, player.position.x, player.position.y, player.half_size.x * 2.0f, player.half_size.y * 2.0f, 0.25f, 0.7f, 1.0f, 1.0f);

				for (int i = 0; i < k_obstacle_count; ++i)
				{
					Obstacle* obstacle = &obstacles[i];
					drawQuad(unit_square, obstacle->position.x, obstacle->position.y, obstacle->half_size.x * 2.0f, obstacle->half_size.y * 2.0f, 1.0f, 0.4f, 0.35f, 1.0f);
				}

				drawHealthBar(unit_square, &player);
			}

			AEGfxSetBlendMode(AE_GFX_BM_BLEND);
			if (font_id >= 0)
			{
				char timer_text[64];
				if (game_state == GAME_STATE_PLAYING)
				{
					f32 time_left = k_stage_duration - stage_timer;
					if (time_left < 0.0f)
						time_left = 0.0f;
					sprintf_s(timer_text, "TIME LEFT: %.1f", time_left);
					AEGfxPrint(font_id, timer_text, -0.95f, 0.85f, 0.45f, 0.9f, 0.9f, 0.9f, 1.0f);
				}

				if (game_state == GAME_STATE_MENU)
					drawMenu(font_id);
				else if (game_state == GAME_STATE_GAME_OVER)
					drawGameOver(font_id);
				else if (game_state == GAME_STATE_VICTORY)
					drawVictory(font_id);
			}

			AESysFrameEnd();
		}

	if (font_id >= 0)
		AEGfxDestroyFont(font_id);
	if (unit_square)
		AEGfxMeshFree(unit_square);

	// free the system
	AESysExit();
}

