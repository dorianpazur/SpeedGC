#include "InputManager.h"

#include <vector>
#include <cstdio>

#include <EABase/eabase.h>

#ifdef EA_PLATFORM_GAMECUBE
#include <gccore.h>
#include <ogc/system.h>
#endif

static bool s_shouldReset = false;
static std::vector<InputCommand, tStdAllocator<InputCommand>> s_commands;
static bool s_controllerConnected[4] = { true, true, true, true };

void InputManager::Initialize()
{
	s_shouldReset = false;
	s_commands.clear();
	for (int i = 0; i < 4; ++i)
	{
		s_controllerConnected[i] = true;
	}
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

const std::vector<InputCommand, tStdAllocator<InputCommand>>& InputManager::GetCommands()
{
	return s_commands;
}

bool InputManager::IsControllerConnected(int playerIndex)
{
#ifdef EA_PLATFORM_GAMECUBE
	if (playerIndex < 0 || playerIndex > 3)
		return false;
	return s_controllerConnected[playerIndex];
#else
	(void)playerIndex;
	return true;
#endif
}

void InputManager::UpdateGameCubeInputs()
{
#ifdef EA_PLATFORM_GAMECUBE

	// PAD_ScanPads returns a bitmask
	// bit set = controller present on that channel
	u32 padMask = PAD_ScanPads();

	s_shouldReset = false;
	s_commands.clear();

	// updtae controller connection state and push ControllerDisconnected on transition
	for (int i = 0; i < 1; ++i)
	{
		//      if controller 1 is connected using bitmask  || 
		bool connected = (padMask & PAD_CHAN_BIT(i)) != 0  || (i == 0 && padMask != 0);
		if (s_controllerConnected[i] && !connected) //detect the disconnection
		{
			s_controllerConnected[i] = false;
			printf("Controller %d disconnected.\n", i + 1);

			//create disconnected inputCommand and push it back to the commands 
			InputCommand dcCmd;
			dcCmd.type = InputCommandType::ControllerDisconnected;
			dcCmd.playerIndex = i;
			dcCmd.value = 1.0f;
			s_commands.push_back(dcCmd);
		}
		else if (!s_controllerConnected[i] && connected)
		{
			s_controllerConnected[i] = true;
			printf("Controller %d reconnected.\n", i + 1);
		}
	}

	// Hardware reset button + special combo (only if controller 0 connected)
	// never call PAD_* for disconnected channels (avoids invalid read on dolphin)
	int buttonsPressed0 = 0;
	if (padMask & PAD_CHAN_BIT(0))
		buttonsPressed0 = PAD_ButtonsHeld(0);
	if (s_controllerConnected[0] &&
		(SYS_ResetButtonDown() ||
			(buttonsPressed0 & PAD_TRIGGER_R &&
				buttonsPressed0 & PAD_TRIGGER_Z &&
				buttonsPressed0 & PAD_BUTTON_B &&
				buttonsPressed0 & PAD_BUTTON_START)))
	{
		s_shouldReset = true;

		InputCommand resetCmd;
		resetCmd.type = InputCommandType::ResetRequested;
		resetCmd.playerIndex = 0;
		resetCmd.value = 1.0f;
		s_commands.push_back(resetCmd);
	}

	// get input for all 4 players and build commands directly (skip disconnected
	for (int i = 0; i < 1; ++i)
	{
		if (!s_controllerConnected[i])
		{
			continue;
		}
		int buttonsDown = PAD_ButtonsDown(i);
		s8  stickX = PAD_StickX(i);  //signed 8bit int for steering
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

