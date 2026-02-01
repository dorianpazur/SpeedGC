#include "FloatWidget.h"

FloatWidget::FloatWidget(const char* name, float* var, float rmin, float rmax, float inc, void(*f)()) : MenuWidget(name, 20, 0, f)
{
    rangeMin = rmin;
    rangeMax = rmax;
    increment = inc;
    theVar = var;
}

void FloatWidget::render(IMenuRender* renderer, int x, int y)
{
    char tmp[176];

    renderer->print(x, y, textColor, name);
    sprintf(tmp, "%.3f", *theVar);
    renderer->print(x + 300, y, textColor, tmp);
}

void FloatWidget::onAction(MenuWidget::actions action, bool repeat)
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
