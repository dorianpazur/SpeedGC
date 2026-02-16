#include "DebugMenuInputHandler.h"
#include "DebugAssistant.h"
#include "DebugMenu.h"
#include "ScreenPrintf.h"
#include "InputManager.h"
#include <gccore.h>

DebugMenuInputHandler::DebugMenuInputHandler()
{
	
}

void DebugMenuInputHandler::PollInput()
{
	// skip pad reads if controller 0 disconnected (avoids invalid read on Dolphin)
	if (!InputManager::IsControllerConnected(0))
		return;

	int buttonsDown = PAD_ButtonsDown(0);
	int buttonsPressed = PAD_ButtonsHeld(0);
	
	if (buttonsPressed & PAD_TRIGGER_Z)
		mActiveShift = true;
	else
		mActiveShift = false;
	
	if (mActiveShift && buttonsDown & PAD_BUTTON_Y)
	{
		if (DebugMenu::isActive())
			Exit();
		else
		{
			DebugMenu::activate();
			Activate();
			DebugMenuLoad();
		}
	}
	
	static bool pressedUp = false;
	if (DebugMenu::isActive() && ((buttonsDown & PAD_BUTTON_UP) || PAD_StickY(0) > 64))
	{
		if (!pressedUp)
			DebugMenu::eventUp();
		pressedUp = true;
	}
	else
	{
		pressedUp = false;
	}
	
	static bool pressedDown = false;
	if (DebugMenu::isActive() && ((buttonsDown & PAD_BUTTON_DOWN) || PAD_StickY(0) < -64))
	{
		if (!pressedDown)
			DebugMenu::eventDown();
		pressedDown = true;
	}
	else
	{
		pressedDown = false;
	}
	
	static bool pressedLeft = false;
	if (DebugMenu::isActive() && ((buttonsDown & PAD_BUTTON_LEFT) || PAD_StickX(0) < -64))
	{
		if (!pressedLeft)
			DebugMenu::eventLeft();
		pressedLeft = true;
	}
	else
	{
		pressedLeft = false;
	}
	
	static bool pressedRight = false;
	if (DebugMenu::isActive() && ((buttonsDown & PAD_BUTTON_RIGHT) || PAD_StickX(0) > 64))
	{
		if (!pressedRight)
			DebugMenu::eventRight();
		pressedRight = true;
	}
	else
	{
		pressedRight = false;
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_A)
	{
		DebugMenu::eventSelect();
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_B)
	{
		DebugMenu::eventCancel();
	}
}

void DebugMenuInputHandler::Activate()
{
    mPrevPrintfState = DoScreenPrintf;

    DoScreenPrintf = true;
}

void DebugMenuInputHandler::Deactivate()
{
    DoScreenPrintf = mPrevPrintfState;
}

void DebugMenuInputHandler::Exit()
{
    if (DebugMenu::isActive())
    {
        DebugMenuInputHandler::Deactivate();
        DebugMenu::deactivate();
        DebugMenu::destroy();
        mActiveShift = false;
    }
}
