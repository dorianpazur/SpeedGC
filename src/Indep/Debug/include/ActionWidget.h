#ifndef DEBUG_ACTIONWIDGET_H
#define DEBUG_ACTIONWIDGET_H

#include "MenuWidget.h"

class ActionWidget : public MenuWidget
{
public:
    ActionWidget(const char* name, void(*f)());
};

#endif