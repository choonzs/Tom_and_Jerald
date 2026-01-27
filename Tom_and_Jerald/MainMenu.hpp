#ifndef MAINMENU_HPP
#define MAINMENU_HPP
void MainMenu_Load();

void MainMenu_Initialize();

void MainMenu_Update();

void MainMenu_Draw();

void MainMenu_Free();

void MainMenu_Unload();
#endif // !MAINMENU_H



/* SCENE TEMPLATE FUNC

// Use to load whatever resources you need for this scene
void MainMenu_Load();

// Use to initialize variables and whatever you need at the start of the scene
// Like setting background, render mode etc
void MainMenu_Initialize();

// Use to update your scene's logic. Make sure to add GAME_STATE_RESTART logic to loop the same scene
void MainMenu_Update();

// Use to draw your scene
void MainMenu_Draw();

// Use to free whatever you allocated in the Initialize function
// Used to free meshes etc 67
void MainMenu_Free();

// Use to unload whatever resources you loaded in the Load function
// Normally used to destroy textures, fonts etc
void MainMenu_Unload();
*/