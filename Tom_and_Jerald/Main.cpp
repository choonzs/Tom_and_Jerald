// ---------------------------------------------------------------------------
// includes test test 123 32323
// main
#include "pch.hpp"
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "Audio.hpp"

// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	srand((unsigned int)time(NULL));

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AESysSetWindowTitle("Tom and Jerald - Prototype");
	AESysReset();

	// initialize all audio assets before the GSM loop starts
	AudioInit();
	// GSM LOOP
		while (current != GAME_STATE_QUIT)
		{

			// when the window is closed
			if (0 == AESysDoesWindowExist()) {
				current = GAME_STATE_QUIT;
			}
			else {
				if (current != GAME_STATE_RESTART) {
					GSM_Update();
					AEFrameRateControllerReset();
					fpLoad();
				}
				else {
					next = previous;
					current = previous;
				}
				fpInitialize();	
				// GAME LOOP
				while (current == next) {
					// Informing the system about the loop's start
					AESysFrameStart();
					PlayBackgroundAudio();
					fpUpdate();
					fpDraw();
					AESysFrameEnd();
				}
				fpFree();
				if (next != GAME_STATE_RESTART) {
					fpUnload();
				}
				previous = current;
				current = next;
			}
		}

	// free all audio assets after the GSM loop ends
	AudioFree();

	// free the system
	AESysExit();
}

