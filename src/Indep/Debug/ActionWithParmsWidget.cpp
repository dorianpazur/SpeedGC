#include "ActionWithParmsWidget.h"

ActionWithParmsWidget::ActionWithParmsWidget(const char* name, void(*f)(void*), void* parm) : MenuWidget(name, 20, 0, nullptr)
{
    callbackWithParm = f;
    parmValue = parm;
}

void ActionWithParmsWidget::onAction(MenuWidget::actions action, bool repeat)
{
    MenuWidget::onAction(action, false);
    if ((!repeat || bCanRepeat) && action == ACTION_SELECT)
    {
        if (callbackWithParm)
            callbackWithParm(parmValue);
    }
}
