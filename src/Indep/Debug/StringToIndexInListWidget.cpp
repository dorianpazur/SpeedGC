#include "StringToIndexInListWidget.h"

StringToIndexInListWidget::StringToIndexInListWidget(const char* name, int& targetValue, const char** labels, int n, int initial_value) : MenuWidget(name, 20, 0, nullptr), mTargetValue(targetValue)
{
    mLabels = labels;
    currentValueIndex = initial_value;
    maxValueIndex = n;
}

void StringToIndexInListWidget::render(IMenuRender* renderer, int x, int y)
{
    renderer->print(x, y, textColor, name);
    renderer->print(x + 300, y, textColor, mLabels[currentValueIndex]);
}

void StringToIndexInListWidget::onAction(MenuWidget::actions action, bool repeat)
{
    MenuWidget::onAction(action, false);
    if (!repeat || bCanRepeat)
    {
        switch (action)
        {
            case ACTION_RIGHT:
            {
                if (currentValueIndex < maxValueIndex - 1)
                    mTargetValue = ++currentValueIndex;
                break;
            }
            case ACTION_LEFT:
            {
                if (currentValueIndex > 0)
                    mTargetValue = --currentValueIndex;
                break;
            }
            default:
                break;
        }
    }
}
