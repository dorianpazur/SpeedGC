#ifndef DEBUG_SUBMENUWIDGET_H
#define DEBUG_SUBMENUWIDGET_H

#include "MenuWidget.h"

struct SubmenuWidget : public MenuWidget
{
    struct DebugMenu* theMenu;

    SubmenuWidget(const char* name, struct DebugMenu* var) : MenuWidget() {
        this->name = name;
        this->theMenu = var;
    };

    virtual void onAction(actions action, bool repeat);
};

#endif