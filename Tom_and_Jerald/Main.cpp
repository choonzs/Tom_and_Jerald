// main
#include "pch.hpp"
#include "GameStateList.hpp"
#include "GameStateManager.hpp"
#include "Audio.hpp"

// For memory leak detection
#define _CRTDBG_MAP_ALLOC
#define _DEBUG_MEM_LEAK // Set to debug for mem leak
#include <crtdbg.h>
// ---------------------------------------------------------------------------
// main

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// Use this for sanity check
	#ifdef _DEBUG_MEM_LEAK
	//(458);
	#endif
	// ====================================
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	// ====================================

	srand((unsigned int)time(NULL));

	// Using custom window procedure
	AESysInit(hInstance, nCmdShow, 1600, 900, 1, 60, false, NULL);
	AESysSetWindowTitle("Tom and Jerald - Prototype");
	AESysReset();

	#ifdef _DEBUG_MEM_LEAK
	_CrtMemState s1;
	_CrtMemCheckpoint(&s1);  // snapshot AFTER engine is done allocating
	#endif
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
			fpInitialize();  // Initialize state variables (positions, timers, meshes)

			// INNER GAME LOOP - runs every frame while state hasn't changed
			while (current == next) {
				AESysFrameStart();      // Signal frame start (updates input, frame timer)
				if (IsMenuKeyTriggered()) {
					clickAudio.Play();
				}
				fpUpdate();             // Run game logic (physics, collisions, input)
				fpDraw();               // Render the current frame
				AESysFrameEnd();        // Signal frame end (enforces frame rate, swaps buffers)
			}
			fpFree(); // Free per-instance resources (meshes, dynamic allocations)

			// Only unload persistent resources if we're not doing a restart
			if (next != GAME_STATE_RESTART) {
				fpUnload();  // Unload textures, fonts, and other loaded assets
			}

			// Track state history for restart functionality
			previous = current;
			current = next;
		}
	}


	// free all audio assets after the GSM loop ends
	AudioFree();

	// free the system
	AESysExit();

	#ifdef _DEBUG_MEM_LEAK
		_CrtMemState s2;
		_CrtMemCheckpoint(&s2);  // snapshot at end

		_CrtMemState diff;
		if (_CrtMemDifference(&diff, &s1, &s2)) {
			_CrtMemDumpStatistics(&diff);  // only shows our leaks & not AE ones
		}
	#endif 
}