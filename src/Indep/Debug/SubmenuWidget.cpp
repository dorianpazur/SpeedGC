#include "SubmenuWidget.h"
#include "DebugAssistant.h"
#include "DebugMenu.h"

void SubmenuWidget::onAction(MenuWidget::actions action, bool repeat)
{
    if (!repeat || bCanRepeat)
    {
        if (action == ACTION_SELECT)
        {
            if ((!repeat || this->bCanRepeat) && action == ACTION_SELECT)
                DebugMenu::changeMenu(theMenu);
        }
    }
}
