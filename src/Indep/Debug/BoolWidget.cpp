#include "BoolWidget.h"

BoolWidget::BoolWidget(const char* name, bool* var, void(*f)()) : MenuWidget(name, 20, 0, f)
{
    theVar = var;
}

void BoolWidget::render(IMenuRender* renderer, int x, int y)
{
    renderer->print(x, y, textColor, name);
    renderer->print(x + 300, y, textColor, *theVar ? "yes" : "no");
}

void BoolWidget::onAction(MenuWidget::actions action, bool repeat)
{
    MenuWidget::onAction(action, false);
    if ((!repeat || bCanRepeat) && action < ACTION_SELECT)
        *theVar = !*theVar;
}
