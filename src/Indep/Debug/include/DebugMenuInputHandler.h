#ifndef DEBUG_DEBUGMENUINPUTHANDLER_H
#define DEBUG_DEBUGMENUINPUTHANDLER_H

#include <tWare/Memory.h>

class DebugMenuInputHandler 
{
public:
    bool mActiveSpeed = false;
    bool mPrevPrintfState = false;
    bool mPrevFEDrawState = true;
    bool mPauseWorld = false;
private:
    bool mActiveShift = false;

public:
	DEF_TWARE_NEW_OVERRIDE(DebugMenuInputHandler, MAIN_POOL)
    DebugMenuInputHandler();
    void PollInput();
    void Exit();
    bool IsDebugKeyPressed() const { return mActiveShift; };
private:
    void Activate();
    void Deactivate();
};

#endif