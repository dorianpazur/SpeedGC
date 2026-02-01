#include "StringToIndexInListWithCallbackWidget.h"

StringToIndexInListWithCallbackWidget::StringToIndexInListWithCallbackWidget(const char* name, void(*f)(int), const char** labels, int n, int initial_value, bool callAtSelection) : MenuWidget(name, 20, 0, nullptr)
{
	this->name = name;
	callback = f;
	mLabels = labels;
	maxValueIndex = n;
	currentValueIndex = 0;
	mCallAtSelection = false;
}

void StringToIndexInListWithCallbackWidget::render(IMenuRender* renderer, int x, int y)
{
	renderer->print(x, y, textColor, name);
	renderer->print(x + 300, y, textColor, mLabels[currentValueIndex]);
}

void StringToIndexInListWithCallbackWidget::onAction(MenuWidget::actions action, bool repeat)
{
    MenuWidget::onAction(action, false);
    if (!repeat || bCanRepeat)
    {
        switch (action)
        {
            case ACTION_SELECT:
            {
                if (mCallAtSelection && callback)
                    callback(currentValueIndex);
                break;
            }
            case ACTION_RIGHT:
            {
                if (currentValueIndex < maxValueIndex - 1)
                    ++currentValueIndex;
                break;
            }
            case ACTION_LEFT:
            {
                if (currentValueIndex > 0)
                    --currentValueIndex;
                break;
            }
            default:
                break;
        }

        if (!mCallAtSelection && callback)
            callback(currentValueIndex);
    }
}
