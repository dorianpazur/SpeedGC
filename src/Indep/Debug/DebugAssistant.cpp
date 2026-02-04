#include "DebugAssistant.h"
#include "DebugMenuRender.h"
#include "DebugMenu.h"
#include "BoolWidget.h"
#include "IntWidget.h"
#include "ActionWidget.h"
#include "FloatWidget.h"
#include "StringListWidget.h"
#include "ScreenPrintf.h"

extern int twkVblankCount;
extern bool bSplitScreen;
extern bool bWideScreen;
extern bool twkDeflicker;

void DebugMenuAllocate()
{
    DebugMenu::setRenderer(new DebugMenuRender());
    DebugMenu::StartNewList();
}

void DebugMenuInit()
{
    gDebugMenuIOHandler = new DebugMenuInputHandler();
    DebugMenuAllocate();
}

void ExitDebugMenu()
{
    gDebugMenuIOHandler->Exit();
}

// this function is for retail, some stuff changed since a124
void DebugMenuLoad()
{
    DebugMenu* root = DebugMenu::create("rootMenu", NULL);

    // front end menu
    DebugMenu* frontend_menu = DebugMenu::create("Front End ->", root); {
        
        DebugMenu* printfs = DebugMenu::create("Printfs ->", frontend_menu); {
            DebugMenu::addWidgetToNewestMenu(new BoolWidget("Screen Printf", &gDebugMenuIOHandler->mPrevPrintfState, NULL));
            DebugMenu::addWidgetToNewestMenu(new BoolWidget("Soak Timer", &DebugMenu::mDoSoakTimer, NULL));
		} DebugMenu::endNewestMenu();

    } DebugMenu::endNewestMenu();
    
	
    // race menu
    DebugMenu* race = DebugMenu::create("Race ->", root); {
        
    } DebugMenu::endNewestMenu();

    DebugMenu* pod = DebugMenu::create("Pod ->", root); {
        
    } DebugMenu::endNewestMenu();

    // rendering menu
    DebugMenu* rendering = DebugMenu::create("Rendering ->", root); {
		DebugMenu::addWidgetToNewestMenu(new IntWidget("VSync", &twkVblankCount, 0, 2, 1, NULL));
		DebugMenu::addWidgetToNewestMenu(new BoolWidget("Widescreen", &bWideScreen, NULL));
		DebugMenu::addWidgetToNewestMenu(new BoolWidget("Splitscreen", &bSplitScreen, NULL));
		DebugMenu::addWidgetToNewestMenu(new BoolWidget("Deflicker", &twkDeflicker, NULL));
    } DebugMenu::endNewestMenu();

    if (gLastCurMenuName)
        DebugMenu::SetCurrentMenu();
}

bool DebugMenuShouldPauseWorld()
{
    if (!gDebugMenuIOHandler)
        return false;

    if (gDebugMenuIOHandler->mPauseWorld || DebugMenu::isActive())
        return true;
    else
        return false;
}
