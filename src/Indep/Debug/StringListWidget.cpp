#include "StringListWidget.h"

StringListWidget::StringListWidget(const char* name, StringListParam* values, int n, int initial_value) : MenuWidget(name, 20, 0, nullptr)
{
    strings = values;
    currentValue = initial_value;
    maxValue = n;
}

void StringListWidget::render(IMenuRender* renderer, int x, int y)
{
    renderer->print(x, y, textColor, name);
    renderer->print(x + 300, y, textColor, strings[currentValue].string);
}

void StringListWidget::onAction(MenuWidget::actions action, bool repeat)
{
    MenuWidget::onAction(action, false);

    if (!repeat || bCanRepeat)
    {
        switch (action)
        {
            case ACTION_RIGHT:
            {
                if (currentValue < maxValue - 1)
                    ++currentValue;
                break;
            }
            case ACTION_LEFT:
            {
                if (currentValue > 0)
                    --currentValue;
                break;
            }
            default:
                break;
        }

        strings[currentValue].f();
    }
}

void StringListWidget::setValue(const char* value)
{
    for (int i = 0; i < maxValue; i++)
    {
        if (strcmp(strings[i].string, value) == 0)
        {
            currentValue = i;
        }
    }
}
