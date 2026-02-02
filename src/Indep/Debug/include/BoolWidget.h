#pragma once

#include "MenuWidget.h"

#ifndef BOOLWIDGET_H
#define BOOLWIDGET_H

struct BoolWidget : public MenuWidget
{
private:
    bool* theVar;
public:
    virtual void onAction(MenuWidget::actions action, bool repeat) override;
    virtual void render(IMenuRender* renderer, int x, int y) override;
    bool* getValue() const { return theVar; }
    void setValue(bool value) { *theVar = value; }
    BoolWidget(const char* name, bool* var, void(*f)());
};

#endif