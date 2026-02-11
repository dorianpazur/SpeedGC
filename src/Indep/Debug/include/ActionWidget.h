#ifndef DEBUG_ACTIONWIDGET_H
#define DEBUG_ACTIONWIDGET_H

#include "MenuWidget.h"

class ActionWidget : public MenuWidget
{
public:
	DEF_TWARE_NEW_OVERRIDE(ActionWidget, MAIN_POOL)
    ActionWidget(const char* name, void(*f)());
};

#endif