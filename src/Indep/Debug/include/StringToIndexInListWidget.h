#ifndef DEBUG_STRINGTOINDEXINLISTWIDGET_H
#define DEBUG_STRINGTOINDEXINLISTWIDGET_H

#include "MenuWidget.h"

class StringToIndexInListWidget : public MenuWidget
{
private:
    int& mTargetValue;
    const char** mLabels;
    int currentValueIndex;
    int maxValueIndex;
public:
	DEF_TWARE_NEW_OVERRIDE(StringToIndexInListWidget, MAIN_POOL)
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    virtual void render(IMenuRender* renderer, int x, int y) override;

    StringToIndexInListWidget(const char* name, int& targetValue, const char** labels, int n, int initial_value);
};

#endif