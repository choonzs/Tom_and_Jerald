#include "pch.h"
#include "Playing.h"

void handlePlayerMovement(Player* player, f32 delta_time)
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