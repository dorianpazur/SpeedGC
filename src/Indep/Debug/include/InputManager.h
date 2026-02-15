#ifndef MISC_INPUTMANAGER_H
#define MISC_INPUTMANAGER_H

#include <EABase/eabase.h>
#include <vector>
#include "InputCommand.h"
#include <tWare/Memory.h>

// InputManager: handles reading controller input
// now it's focused on GameCube but the interface can be extended later.

struct PlayerInputState
{
	float throttle;     // 0..1 from R trigger
	float brake;        // 0..1 from L trigger
	float steering;     // -1..1 from stick X
	bool  startPressed; // true when (start) was pressed this frame
};

class InputManager
{
public:
	DEF_TWARE_NEW_OVERRIDE(InputManager, MAIN_POOL)
	static void Initialize();
	static void Update(); // call once per frame before using input

	static const PlayerInputState& GetPlayerState(int index);

	// reset logic  as old UpdatePlatform
	static bool ShouldReset();

	// Updated every call to Update()
	static const std::vector<InputCommand, tStdAllocator<InputCommand>>& GetCommands();

private:
	static void UpdateGameCubeInputs();
};

#endif


