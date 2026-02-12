#ifndef DEBUG_DEBUGMENU_H
#define DEBUG_DEBUGMENU_H

#include <tWare/List.h>
#include "IMenuRender.h"
#include "MenuWidget.h"
#include "DebugMenuInputHandler.h"

extern void DebugMenuInit();
extern const char* gLastCurMenuName;
extern int gLastCurSelection;

struct DebugMenuAlloc;

struct DebugMenu : tTNode<DebugMenu>
{
    static bool mDoSoakTimer;
    static unsigned int mSoakTicker;
private:
    DebugMenu* mParent;
    int mOriginX;
    int mOriginY;
    int mWidth;
    int mHeight;
    uint32_t mBackgroundColor;
    int mCurrentSelection;
    int mWidgetToRenderFrom;
    const char* mName;
    tTList<struct MenuWidget> mItems;
    static bool mActive;
    static IMenuRender* mRenderer;
    static struct DebugMenu* mRootMenu;
    static struct DebugMenu* mCurrentMenu;
    static struct DebugMenu* mNewestMenu;
    static tTList<DebugMenuAlloc>* mAllocations;

public:
	DEF_TWARE_NEW_OVERRIDE(DebugMenu, MAIN_POOL)
    DebugMenu();
    ~DebugMenu();
    static DebugMenu* create(const char* name, DebugMenu* parent);
    static void destroy();
    static void StartNewList();
    static void renderBackground();
    static void render();
    static IMenuRender* getRenderer() { return mRenderer; };
    const char* GetName() { return mName; };
    int GetCurrentSelection() { return mCurrentSelection; };
    void SetWidgetToRenderFrom(int widget) { mWidgetToRenderFrom = widget; };
    void SetCurrentSelection(int selection) { mCurrentSelection = selection; };
    static void setRenderer(IMenuRender* renderer) { mRenderer = renderer; };
    DebugMenu* GetParent() { return mParent; };
    static void changeMenu(DebugMenu* menu) { mCurrentMenu = menu; };
    static void SetCurrentMenu();
    static void addWidgetToNewestMenu(MenuWidget* widget);
    static void endNewestMenu();
    static void activate();
    static void deactivate();
    static bool isActive();
    static void eventLeft();
    static void eventRight();
    static void eventUp();
    static void eventDown();
    static void eventSelect();
    static void eventCancel();
	static void SoakTimer();
    void addWidget(struct MenuWidget* widget);
    struct MenuWidget* GetWidgetWithFocus();
    void RemoveAllFocus();
private:
    void actionLeft();
    void actionRight();
    void actionUp();
    void actionDown();
    void actionSelect();
    void actionCancel();
    void doRender(IMenuRender* renderer);
    void doRenderBackground(IMenuRender* renderer) const;
    bool selectionFitsOnScreen();
    MenuWidget* getWidget(int n);
};

struct DebugMenuAlloc : tTNode<DebugMenuAlloc>
{
	DEF_TWARE_NEW_OVERRIDE(DebugMenuAlloc, MAIN_POOL)
    DebugMenu* menu;
	~DebugMenuAlloc()
	{
		if (menu)
		{
			delete menu;
			menu = NULL;
		}
	}
};

extern DebugMenuInputHandler* gDebugMenuIOHandler;

#endif
