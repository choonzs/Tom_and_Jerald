#include "pch.hpp"
#include "Player.hpp"
#include "Playing.hpp"
#include "Utils.hpp"
#include "LevelTile.hpp"


f32 randFloat(f32 min, f32 max) {
	return min + (max - min) * ((f32)rand() / RAND_MAX);
}

// Bruh why do we have two functions that do the same thing?
f32 randomRange(f32 min_value, f32 max_value)
{
	return min_value + (max_value - min_value) * (rand() / (f32)RAND_MAX);
}


void drawCenteredText(s8 font_id, const char* text, f32 y, f32 scale, f32 cam_pos_x, f32 cam_pos_y,f32 red, f32 green, f32 blue, f32 alpha)
{
	f32 width = 0.0f;
	f32 height = 0.0f;
	AEGfxGetPrintSize(font_id, text, scale, &width, &height);
	AEGfxPrint(font_id, text, -width * 0.5f + cam_pos_x, y + cam_pos_y, scale, red, green, blue, alpha);
}


void drawText(s8 font_id, const char* text, f32 scale, f32 cam_pos_x, f32 cam_pos_y)
{
	f32 width = 0.0f;
	f32 height = 0.0f;
	AEGfxGetPrintSize(font_id, text, scale, &width, &height);
	AEGfxPrint(font_id, text, cam_pos_x, cam_pos_y, scale, 1.0f, 1.0f, 1.0f, 1.0f);
}


void createUnitSquare(AEGfxVertexList** out_mesh, f32 sprite_uv_height, f32 sprite_uv_width)
{
	AEGfxMeshStart();
	AEGfxTriAdd(
		-0.5f, -0.5f, 0xFFFFFFFF, 0.0f, sprite_uv_height, // bottom left
		0.5f, -0.5f, 0xFFFFFFFF, sprite_uv_width, sprite_uv_height, // bottom right 
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // top left

	AEGfxTriAdd(
		0.5f, -0.5f, 0xFFFFFFFF, sprite_uv_width, sprite_uv_height, // bottom right 
		0.5f, 0.5f, 0xFFFFFFFF, sprite_uv_width, 0.0f,   // top right
		-0.5f, 0.5f, 0xFFFFFFFF, 0.0f, 0.0f);  // bottom left

	*out_mesh = AEGfxMeshEnd();
}

void createUnitCircles(AEGfxVertexList** out_mesh) {
	AEGfxMeshStart();
		f32 radius = 0.5f;
		u8 segments = 100;
	float angle_increment = (2.0f * 3.14159f) / segments;
	for (u8 i = 0; i < segments; ++i) {
		float angle1 = i * angle_increment;
		float angle2 = (i + 1) % segments * angle_increment;
		float x1 = radius * cosf(angle1);
		float y1 = radius * sinf(angle1);
		float x2 = radius * cosf(angle2);
		float y2 = radius * sinf(angle2);
		AEGfxTriAdd(
			0.0f, 0.0f, 0xFFFFFFFF, 0.5f, 0.5f,
			x1, y1, 0xFFFFFFFF, (cosf(angle1) + 1.0f) * 0.5f, (sinf(angle1) + 1.0f) * 0.5f,
			x2, y2, 0xFFFFFFFF, (cosf(angle2) + 1.0f) * 0.5f, (sinf(angle2) + 1.0f) * 0.5f
		);
	}
	// Saving the mesh (list of triangles) in pMesh
	*out_mesh = AEGfxMeshEnd();
}


void drawQuad(AEGfxVertexList* mesh, f32 center_x, f32 center_y, f32 width, f32 height, f32 red, f32 green, f32 blue, f32 alpha)
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


bool checkOverlap(const AEVec2* position_a, const AEVec2* half_size_a, const AEVec2* position_b, const AEVec2* half_size_b)
{

	if (fabsf(position_a->x - position_b->x) > (half_size_a->x + half_size_b->x))
		return false;

	if (fabsf(position_a->y - position_b->y) > (half_size_a->y + half_size_b->y))
		return false;

	return true;
}


void drawHealthBar(AEGfxVertexList* mesh, const Player& player, int max_health)
{
	f32 min_x = AEGfxGetWinMinX();
	f32 max_y = AEGfxGetWinMaxY();
	f32 bar_width = 32.0f;
	f32 bar_height = 12.0f;
	f32 spacing = 6.0f;
	f32 start_x = min_x + 30.0f;
	f32 start_y = max_y - 30.0f;

	for (int i{}; i < max_health; ++i)
	{
		f32 center_x = start_x + (bar_width + spacing) * i;
		f32 center_y = start_y;
		drawQuad(mesh, center_x, center_y, bar_width, bar_height, 0.15f, 0.15f, 0.15f, 1.0f);
		if (i < player.Health())
			drawQuad(mesh, center_x, center_y, bar_width - 4.0f, bar_height - 4.0f, 0.2f, 0.9f, 0.35f, 1.0f);
	}
}

void LoadLevelDataFromFile(std::string filename, f32& level_end_x,std::vector<LevelTile>& map_tiles, ObstacleSystem& obstacle_system) {
	std::ifstream inFile(filename);
	if (inFile.is_open()) {
		int cols, rows;
		inFile >> cols >> rows;

		/*f32 halfW = AEGfxGetWinMaxX();
		f32 halfH = AEGfxGetWinMaxY();
		f32 TILE_SIZE = (halfH * 2.0f) / rows;
		f32 startX = -halfW;
		f32 startY = -halfH;*/

		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				LevelTile tile;
				if (!(inFile >> tile)) {   // check if read failed / EOF 
					std::cout << "End of File.\n";
					break;
				}

				if (tile.type != 0) {
					/*LevelTile tile{
						type,
						{startX + (c * TILE_SIZE) + TILE_SIZE / 2.0f, startY + (r * TILE_SIZE) + TILE_SIZE / 2.0f},
						{TILE_SIZE / 2.0f, TILE_SIZE / 2.0f}
					};*/
					std::cout << "Read tile from file: type=" << tile.type << ", pos=(" << tile.pos.x << ", " << tile.pos.y << "), half_size=(" << tile.half_size.x << ", " << tile.half_size.y << "), velocity=(" << tile.velocity.x << ", " << tile.velocity.y << "), scale=" << tile.scale << "\n";
					map_tiles.push_back(tile);
					if (tile.pos.x > level_end_x) level_end_x = tile.pos.x;
					//std::cout << "Loaded tile of type " << tile.type << " at position (" << tile.pos.x << ", " << tile.pos.y << ") with half-size (" << tile.half_size.x << ", " << tile.half_size.y << ") and velocity (" << tile.velocity.x << ", " << tile.velocity.y << ")\n";

					// Creating obstacle object based on tile type and randomizing FOR NOW its size and speed within a range
					Obstacle 
						new_obstacle(
						static_cast<ObstacleType>(tile.type),
						tile.pos,
						tile.velocity,
						{tile.half_size.x * tile.scale, tile.half_size.y * tile.scale}
					);
					if (new_obstacle.Type() == Spike) {
						new_obstacle.Velocity() = { 0.0f, 0.0f }; // Spikes don't move
					}

					std::cout << "Loaded obstacle of type " << new_obstacle.Type() << " at position (" << new_obstacle.Position().x << ", " << new_obstacle.Position().y << ") with velocity (" << new_obstacle.Velocity().x << ", " << new_obstacle.Velocity().y << ") and half-size (" << new_obstacle.HalfSize().x << ", " << new_obstacle.HalfSize().y << ")\n";
					// Add it to the obstacle system's vector
					obstacle_system.AddObstacle(new_obstacle);
				}
			}
		}
		_CrtDumpMemoryLeaks(); // prints currently allocated memory
		inFile.close();
	}
	level_end_x = 1000.0f; // Adding some buffer to the end of the level
	//std::cout << "Level data loaded from file: " << filename << " with " << map_tiles.size() << " tiles and " << obstacle_system.Obstacles().size() << " obstacles.\n";
}

