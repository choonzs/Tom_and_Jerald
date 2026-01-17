// ---------------------------------------------------------------------------
// includes test test 123 32323
// main
#include "pch.h"
#include "GameStateList.h"
#include "GameStateManager.h"

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
	// GSM LOOP
		while (current != GAME_STATE_QUIT)
		{
			// Informing the system about the loop's start
			AESysFrameStart();

			// when ESCAPE is hit or when the window is closed
			if (AEInputCheckTriggered(AEVK_ESCAPE) || 0 == AESysDoesWindowExist()) {
				current = GAME_STATE_QUIT;
			}
			else {
				if (current != GAME_STATE_RESTART) {
					GSM_Update();
					fpLoad();
				}
				else {
					next = previous;
					current = previous;
				}
				fpInitialize();
				// GAME LOOP
				while (current == next) {
					fpUpdate();
					fpDraw();
				}
				fpFree();
				if (next != GAME_STATE_RESTART) {
					fpUnload();
				}
				previous = current;
				current = next;
			}
			AESysFrameEnd();
		}
	// free the system
	AESysExit();
}

