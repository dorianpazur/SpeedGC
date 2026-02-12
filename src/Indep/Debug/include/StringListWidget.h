#ifndef DEBUG_STRINGLISTWIDGET_H
#define DEBUG_STRINGLISTWIDGET_H

#include "MenuWidget.h"

struct StringListParam
{
    const char* string;
    void  (*f)();
};

class StringListWidget : public MenuWidget
{
private:
    StringListParam* strings;
    int currentValue;
    int maxValue;
public:
	DEF_TWARE_NEW_OVERRIDE(StringListWidget, MAIN_POOL)
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    virtual void render(IMenuRender* renderer, int x, int y) override;
    int getCurrentValue() const { return currentValue; }
    void setValue(const char*);
    StringListWidget(const char* name, StringListParam* values, int n, int initial_value);
};

#endif