#ifndef DEBUG_FLOATWIDGET_H
#define DEBUG_FLOATWIDGET_H

#include "MenuWidget.h"

class FloatWidget : public MenuWidget
{
private:
    float rangeMin;
    float rangeMax;
    float increment;
    float* theVar;
public:
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    virtual void render(IMenuRender* renderer, int x, int y) override;
    float* getValue() const { return theVar; }
    void setValue(float value) { *theVar = value; }
    FloatWidget(const char* name, float* var, float rmin, float rmax, float inc, void(*f)());
};

#endif