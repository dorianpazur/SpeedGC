#ifndef DEBUG_DEBUGMENUINPUTHANDLER_H
#define DEBUG_DEBUGMENUINPUTHANDLER_H

#include "DebugMenuInputHandler.h"

class DebugMenuInputHandler 
{
public:
    bool mActiveSpeed = false;
    bool mPrevPrintfState = false;
    bool mPrevFEngDrawState = true;
    bool mPauseWorld = false;
private:
    bool mActiveShift = false;

public:
    DebugMenuInputHandler();
    void PollInput();
    void Exit();
    bool IsDebugKeyPressed() const { return mActiveShift; };
private:
    void Activate();
    void Deactivate();
};

#endif