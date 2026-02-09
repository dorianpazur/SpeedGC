#include "InputManager.h"

#include <vector>

#include <EABase/eabase.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <gccore.h>
#include <ogc/system.h>
#endif

static bool s_shouldReset = false;
static std::vector<InputCommand> s_commands;

void InputManager::Initialize()
{
	s_shouldReset = false;
	s_commands.clear();
}

void InputManager::Update()
{
#ifdef EA_PLATFORM_GAMECUBE
	UpdateGameCubeInputs();
#else
	s_commands.clear(); //for other platforms in the future
#endif
}

bool InputManager::ShouldReset()
{
	return s_shouldReset;
}

const std::vector<InputCommand>& InputManager::GetCommands()
{
	return s_commands;
}

void InputManager::UpdateGameCubeInputs()
{
#ifdef EA_PLATFORM_GAMECUBE
	PAD_ScanPads();

	// Hardware reset button + special combo
	int buttonsPressed0 = PAD_ButtonsHeld(0);

	s_shouldReset = false;
	s_commands.clear();

	if (SYS_ResetButtonDown() ||
		(buttonsPressed0 & PAD_TRIGGER_R &&
			buttonsPressed0 & PAD_TRIGGER_Z &&
			buttonsPressed0 & PAD_BUTTON_B &&
			buttonsPressed0 & PAD_BUTTON_START))
	{
		s_shouldReset = true;

		InputCommand resetCmd;
		resetCmd.type = InputCommandType::ResetRequested;
		resetCmd.playerIndex = 0;
		resetCmd.value = 1.0f;
		s_commands.push_back(resetCmd);
	}

	// get input for all 4 players and build commands directly
	for (int i = 0; i < 4; ++i)
	{
		int buttonsDown = PAD_ButtonsDown(i);
		s8  stickX = PAD_StickX(i);
		u8  triggerL = PAD_TriggerL(i);
		u8  triggerR = PAD_TriggerR(i);

		// raw input to normalized values
		float throttle = triggerR / 255.0f;
		float brake = triggerL / 255.0f;
		float steering = stickX / 127.0f;
		bool  startPressed = (buttonsDown & PAD_BUTTON_START) != 0;

		// build commands directly from raw input
		if (throttle > 0.0f)
		{
			InputCommand cmd;
			cmd.type = InputCommandType::Accelerate;
			cmd.playerIndex = i;
			cmd.value = throttle;
			s_commands.push_back(cmd);
		}

		if (brake > 0.0f)
		{
			InputCommand cmd;
			cmd.type = InputCommandType::Brake;
			cmd.playerIndex = i;
			cmd.value = brake;
			s_commands.push_back(cmd);
		}

		// Only create steer command if outdide deadzones
		if (steering > 0.05f || steering < -0.05f)
		{
			InputCommand cmd;
			cmd.type = InputCommandType::Steer;
			cmd.playerIndex = i;
			cmd.value = steering;
			s_commands.push_back(cmd);
		}

		if (startPressed)
		{
			InputCommand cmd;
			cmd.type = InputCommandType::StartPressed;
			cmd.playerIndex = i;
			cmd.value = 1.0f;
			s_commands.push_back(cmd);
		}
	}
#endif
}

