#include "ActionWidget.h"

ActionWidget::ActionWidget(const char* name, void(*f)()) : MenuWidget(name, 20, 0, f)
{
    // yep, that's all there is to it...
    this->bExitMenuAfterSelected = true;
}
