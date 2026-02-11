#pragma once

#include "MenuWidget.h"

#ifndef INTWIDGET_H
#define INTWIDGET_H

class IntWidget : public MenuWidget
{
private:
    int rangeMin;
    int rangeMax;
    int increment;
    int* theVar;
public:
	DEF_TWARE_NEW_OVERRIDE(IntWidget, MAIN_POOL)
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    virtual void render(IMenuRender* renderer, int x, int y) override;
    int* getValue() const { return theVar; }
    void setValue(int value) { *theVar = value; }
    IntWidget(const char* name, int* var, int rmin, int rmax, int inc, void(*f)());
};

#endif