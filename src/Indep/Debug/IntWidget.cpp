#include "IntWidget.h"

IntWidget::IntWidget(const char* name, int* var, int rmin, int rmax, int inc, void(*f)()) : MenuWidget(name, 20, 0, f)
{
    rangeMin = rmin;
    rangeMax = rmax;
    increment = inc;
    theVar = var;
}

void IntWidget::render(IMenuRender* renderer, int x, int y)
{
    char tmp[176];

    renderer->print(x, y, textColor, name);
    sprintf(tmp, "%d", *theVar);
    renderer->print(x + 300, y, textColor, tmp);
}

void IntWidget::onAction(MenuWidget::actions action, bool repeat)
{
    MenuWidget::onAction(action, false);

    if (!repeat || bCanRepeat)
    {
        switch (action)
        {
            case ACTION_RIGHT:
            {
                if (*theVar + increment <= rangeMax)
                    *theVar += increment;
                break;
            }
            case ACTION_LEFT:
            {
                if (*theVar - increment >= rangeMin)
                    *theVar -= increment;
                break;
            }
            default:
                break;
        }
    }
}
