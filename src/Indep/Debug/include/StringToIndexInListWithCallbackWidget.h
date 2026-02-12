#ifndef DEBUG_STRINGTOINDEXINLISTWITHCALLBACKWIDGET_H
#define DEBUG_STRINGTOINDEXINLISTWITHCALLBACKWIDGET_H

#include "MenuWidget.h"

class StringToIndexInListWithCallbackWidget : public MenuWidget
{
private:
    void  (*callback)(int);
    const char** mLabels;
    int currentValueIndex;
    int maxValueIndex;
    bool mCallAtSelection;
public:
	DEF_TWARE_NEW_OVERRIDE(StringToIndexInListWithCallbackWidget, MAIN_POOL)
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    virtual void render(IMenuRender* renderer, int x, int y) override;

    StringToIndexInListWithCallbackWidget(const char* name, void(*f)(int), const char** labels, int n, int initial_value, bool callAtSelection);
};

#endif