#include "DebugMenuInputHandler.h"
#include "DebugAssistant.h"
#include "DebugMenu.h"
#include "ScreenPrintf.h"
#include <gccore.h>

DebugMenuInputHandler::DebugMenuInputHandler()
{
	
}

void DebugMenuInputHandler::PollInput()
{
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
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_UP)
	{
		DebugMenu::eventUp();
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_DOWN)
	{
		DebugMenu::eventDown();
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_LEFT)
	{
		DebugMenu::eventLeft();
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_RIGHT)
	{
		DebugMenu::eventRight();
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_A)
	{
		DebugMenu::eventSelect();
	}
	
	if (DebugMenu::isActive() && buttonsDown & PAD_BUTTON_B)
	{
		DebugMenu::eventCancel();
	}
	
	//while (!mActionQ->IsEmpty())
    //{
    //    ActionRef ref;
    //    mActionQ->GetAction(&ref);
	//
    //    switch (ref.actiondata->id)
    //    {
    //        case FRONTENDACTION_UP:
    //        case FRONTENDACTION_DOWN:
    //            if (ref.actiondata->data == 1.0f)
    //            {
    //                if (ref.actiondata->id == FRONTENDACTION_UP)
    //                    DebugMenu::eventUp();
    //                else
    //                    DebugMenu::eventDown();
    //            }
    //            break;
    //        case FRONTENDACTION_LEFT:
    //            if (DebugMenu::isActive())
    //                DebugMenu::eventLeft();
    //            break;
    //        case FRONTENDACTION_RIGHT:
    //            if (DebugMenu::isActive())
    //                DebugMenu::eventRight();
    //            break;
    //        case FRONTENDACTION_ACCEPT:
    //            if (DebugMenu::isActive())
    //                DebugMenu::eventSelect();
    //            break;
    //        case FRONTENDACTION_CANCEL:
    //            if (DebugMenu::isActive())
    //                DebugMenu::eventCancel();
    //            // xbox/xbox 360
    //            /*
    //            else if (mActiveSpeed)
    //                TriggerSetSpeed();
    //            */
    //            break;
    //        //case FRONTENDACTION_CANCEL_ALT: // PS2
    //        case FRONTENDACTION_BUTTON2: // PC????
    //            if (!DebugMenu::isActive() && mActiveSpeed)
    //                TriggerSetSpeed();
    //            break;
    //        case FRONTENDACTION_LTRIGGER:
    //            if (mActiveShift && ref.actiondata->data == 1.0f)
    //            {
    //                if (DebugMenu::isActive())
    //                    Exit();
    //                else
    //                {
    //                    DebugMenu::activate();
    //                    Activate();
    //                    DebugMenuLoad();
    //                }
    //            }
    //            break;
    //        case FRONTENDACTION_RTRIGGER:
    //            // shift button
    //            if (ref.actiondata->data == 1.0f)
    //                mActiveShift = true;
    //            else if (ref.actiondata->data == 0.0f)
    //                mActiveShift = false;
	//
    //            break;
    //    }
    //    mActionQ->PopAction();
    //}
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
