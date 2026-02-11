#ifndef DEBUG_ACTIONWITHPARMSWIDGET_H
#define DEBUG_ACTIONWITHPARMSWIDGET_H

#include "MenuWidget.h"

class ActionWithParmsWidget : public MenuWidget
{
private:
    void (*callbackWithParm)(void*);
    void* parmValue;
public:
	DEF_TWARE_NEW_OVERRIDE(ActionWithParmsWidget)
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    ActionWithParmsWidget(const char* name, void(*f)(void*), void* parm);
};

#endif