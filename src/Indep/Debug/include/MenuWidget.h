#ifndef DEBUG_MENUWIDGET_H
#define DEBUG_MENUWIDGET_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <tWare/Memory.h>
#include <tWare/List.h>
#include "IMenuRender.h"

struct MenuWidget : tTNode<MenuWidget> {
    enum actions : int32_t
    {
        ACTION_LEFT,
        ACTION_RIGHT,
        ACTION_SELECT,
        ACTION_CANCEL,
    };

    int height = 20;
    int width = 0;
    const char* name;
    bool bHasFocus;
    bool bCanRepeat;
    bool bExitMenuAfterSelected;
    unsigned int textColor;
    void(*callbackFunction)();
	
	DEF_TWARE_NEW_OVERRIDE(MenuWidget, MAIN_POOL)
	
    MenuWidget();
    MenuWidget(const char* n, int h, int w, void(*f)());

    const char* getName();
    int getHeight();
    int getWidth();
    bool canRepeat();
    bool hasFocus();
    void setHeight(int h);
    void setWidth(int w);
    void SetExitMenuAfterSelected(bool exit);
    void initializeSize(IMenuRender* renderer);

    virtual ~MenuWidget();
    virtual void onGainFocus();
    virtual void onLoseFocus();
    virtual void render(IMenuRender*, int x, int y);
    virtual void onAction(actions action, bool repeat);
};

#endif