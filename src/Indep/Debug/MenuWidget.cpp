#include "MenuWidget.h"
#include "DebugAssistant.h"

MenuWidget::MenuWidget() {
    // toru: added code in here to make the compiler shut up about uninitialized stuff
    this->height = 0;
    this->width = 0;
    this->name = NULL;
    this->bCanRepeat = true;
    this->textColor = ~0; // white
    this->callbackFunction = NULL;
    this->bExitMenuAfterSelected = false;
    this->bHasFocus = false;
};
MenuWidget::~MenuWidget() {};

MenuWidget::MenuWidget(const char* n, int h, int w, void(*f)())
{
    this->height = h;
    this->width = w;
    this->name = n;
    this->bCanRepeat = true;
    this->textColor = ~0; // white
    this->callbackFunction = f;
    this->bExitMenuAfterSelected = false;
    this->bHasFocus = false;
}

void MenuWidget::render(IMenuRender* renderer, int x, int y)
{
    renderer->print(x, y, textColor, name);
}

void MenuWidget::onAction(MenuWidget::actions action, bool repeat)
{
    if (!repeat || bCanRepeat)
    {
        if (action == ACTION_SELECT)
        {
            if (callbackFunction)
                callbackFunction();

            if (bExitMenuAfterSelected)
                ExitDebugMenu();
        }
    }
}

void MenuWidget::onGainFocus()
{
    bHasFocus = true;
    textColor = 0xFFFFFF00; // yellow
}

void MenuWidget::onLoseFocus()
{
    bHasFocus = false;
    textColor = 0xFFFFFFFF; // white
}

const char* MenuWidget::getName()
{
    return name;
}

int MenuWidget::getHeight()
{
    return height;
}

int MenuWidget::getWidth()
{
    return width;
}

void MenuWidget::setHeight(int h)
{
    height = h;
}

void MenuWidget::setWidth(int w)
{
    width = 350;
}

void MenuWidget::SetExitMenuAfterSelected(bool exit)
{
    bExitMenuAfterSelected = exit;
}

bool MenuWidget::canRepeat()
{
    return bCanRepeat;
}

bool MenuWidget::hasFocus()
{
    return bCanRepeat;
}

void MenuWidget::initializeSize(IMenuRender* renderer)
{
    int x;
    int y;

    renderer->getStringSize(name, &x, &y);
    setHeight(y);
    setWidth(x);
;}
