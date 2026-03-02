#include "pch.hpp"
#include "Player.hpp"
#include "playing.hpp"
#include "LevelTile.hpp"
#include "Obstacle.hpp"

#ifndef UTILS_HPP
#define UTILS_HPP

/*!*****************************************************************************
\brief
	Provides a random float between the specified minimum and maximum values.

\param[in] min
	Minimum value of the random float.

\param[in] max
	Maximum value of the random float.

\return
	A random float between min and max.
*******************************************************************************/
f32 randFloat(f32 min, f32 max);
f32 randomRange(f32 min_value, f32 max_value);

/*!*****************************************************************************
\brief
	Draws centered text on the screen using the specified font, text, position, and scale.

\param[in] font_id
	The ID of the font to use for drawing the text.

\param[in] text
	The text string to be drawn.

\param[in] y
	The y-coordinate for the text's position.

\param[in] scale
	The scale factor for the text size.

\param[in] cam_pos_x
	The x-coordinate of the camera position to adjust the text's position accordingly.

\param[in] cam_pos_y
	The y-coordinate of the camera position to adjust the text's position accordingly.
*******************************************************************************/
void drawCenteredText(s8 font_id, const char* text, f32 y, f32 scale, f32 cam_pos_x = 0.0f, f32 cam_pos_y = 0.0f, f32 red = 1.0f, f32 green = 1.0f, f32 blue = 1.0f, f32 alpha = 1.0f);

/*!*****************************************************************************
\brief
	Draws text on the screen using the specified font, text, position, and scale.

\param[in] font_id
	The ID of the font to use for drawing the text.

\param[in] text
	The text string to be drawn.

\param[in] scale
	The scale factor for the text size.

\param[in] cam_pos_x
	The x-coordinate of the camera position to adjust the text's position accordingly.

\param[in] cam_pos_y
	The y-coordinate of the camera position to adjust the text's position accordingly.
*******************************************************************************/
void drawText(s8 font_id, const char* text, f32 scale, f32 cam_pos_x, f32 cam_pos_y);

/*!*****************************************************************************
\brief
	Creates a unit square mesh with the specified UV coordinates for texturing.

\param[in] out_mesh
	Pointer to an AEGfxVertexList pointer that will receive the created mesh.

\param[in] sprite_uv_height
	The height of the UV coordinates for the sprite texture.

\param[in] sprite_uv_width
	The width of the UV coordinates for the sprite texture.
*******************************************************************************/
void createUnitSquare(AEGfxVertexList** out_mesh, f32 sprite_uv_height = 1.0f, f32 sprite_uv_width = 1.0f);

/*!*****************************************************************************
\brief
	Creates a unit circle mesh

\param[in] out_mesh
	Pointer to an AEGfxVertexList pointer that will receive the created mesh.
*******************************************************************************/
void createUnitCircles(AEGfxVertexList** out_mesh);

/*!*****************************************************************************
\brief
	Draws a quadrilateral (quad) on the screen using the specified mesh, position, size, and color.
\param[in] mesh
	The AEGfxVertexList mesh to be drawn.
\param[in] center_x
	The x-coordinate of the center of the quad.
\param[in] center_y
	The y-coordinate of the center of the quad.
\param[in] width
	The width of the quad.
\param[in] height
	The height of the quad.
\param[in] red
	The red component of the quad's color (0.0f to 1.0f).
\param[in] green
	The green component of the quad's color (0.0f to 1.0f).
\param[in] blue
	The blue component of the quad's color (0.0f to 1.0f).
\param[in] alpha
	The alpha (transparency) component of the quad's color (0.0f to 1.0f).
*******************************************************************************/
void drawQuad(AEGfxVertexList* mesh, f32 center_x, f32 center_y, f32 width, f32 height, f32 red, f32 green, f32 blue, f32 alpha);

/*!*****************************************************************************
\brief
	Checks for overlap (collision) between two axis-aligned bounding boxes (AABBs) defined by their positions and half-sizes.

\param[in] position_a
	The position of the center of the first AABB.
\param[in] half_size_a
	The half-size (half-width and half-height) of the first AABB.
\param[in] position_b
	The position of the center of the second AABB.
\param[in] half_size_b
	The half-size (half-width and half-height) of the second AABB.
*******************************************************************************/
bool checkOverlap(const AEVec2* position_a, const AEVec2* half_size_a, const AEVec2* position_b, const AEVec2* half_size_b);

/*!*****************************************************************************
\brief
	Draws the player's health bar on the screen based on the player's current health and maximum health.
\param[in] mesh
	The AEGfxVertexList mesh to be used for drawing the health bar.

\param[in] player
	The Player object containing the current health information.
\param[in] max_health
	The maximum health value to determine how many health segments to draw.
*******************************************************************************/
void drawHealthBar(AEGfxVertexList* mesh, const Player& player, int max_health);

/*!*****************************************************************************
\brief
	Loads level data from a file and populates the provided vector of LevelTile objects and the ObstacleSystem with obstacles based on the loaded data.
\param[in] filename
	The name of the file containing the level data to be loaded.

\param[in] level_end_x
	A reference to a float that will be updated with the x-coordinate of the end of the level based on the loaded data.
\param[in] out_tiles
	A reference to a vector of LevelTile objects that will be populated with the level tile data loaded from the file.
\param[in] obstacle_system
	A reference to an ObstacleSystem object that will be populated with obstacles based on the loaded level data.

*******************************************************************************/
void LoadLevelDataFromFile(const char* filename, f32& level_end_x, std::vector<LevelTile>& out_tiles, ObstacleSystem& obstacle_system);

#endif // !UTILS_H