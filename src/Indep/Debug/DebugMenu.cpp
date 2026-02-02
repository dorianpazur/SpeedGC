#include "DebugMenu.h"
#include "DebugAssistant.h"
#include "SubmenuWidget.h"
#include "ScreenPrintf.h"

IMenuRender* DebugMenu::mRenderer = NULL;
DebugMenuInputHandler* gDebugMenuIOHandler = NULL;

// debug menu

const char* gLastCurMenuName = NULL;
int gLastCurSelection = 0;
bool DebugMenu::mActive = false;
bool DebugMenu::mDoSoakTimer = false;
unsigned int DebugMenu::mSoakTicker = 0;
DebugMenu* DebugMenu::mRootMenu = NULL;
DebugMenu* DebugMenu::mCurrentMenu = NULL;
DebugMenu* DebugMenu::mNewestMenu = NULL;
tTList<DebugMenuAlloc>* DebugMenu::mAllocations = NULL;
float DebugMenuTransparencyLevel = 1.0f;

DebugMenu::DebugMenu()
{
}

DebugMenu::~DebugMenu()
{
}

void DebugMenu::StartNewList()
{
    DebugMenu::mAllocations = new tTList<DebugMenuAlloc>();
}

DebugMenu* DebugMenu::create(const char* name, DebugMenu* parent)
{
    DebugMenu* newMenu = new DebugMenu();
    DebugMenuAlloc* newAlloc = new DebugMenuAlloc();

    newMenu->mParent = parent;
    newMenu->mName = name;
    newMenu->mBackgroundColor = 0xFF000000;
    newMenu->mOriginX = 100;
    newMenu->mOriginY = 65;
    newMenu->mWidth = 440;
    newMenu->mHeight = 200;
    newMenu->mWidgetToRenderFrom = -1;
    newMenu->mCurrentSelection = -1;

    newAlloc->menu = newMenu;
    DebugMenu::mAllocations->AddTail(newAlloc);

    mNewestMenu = newMenu;

    if (parent)
    {
        parent->addWidget(new SubmenuWidget(name, newMenu));
    }
    else
    {
        if (DebugMenu::mRootMenu == NULL)
        {
            DebugMenu::mRootMenu = DebugMenu::mCurrentMenu = newMenu;
        }
    }

    return newMenu;
}

void DebugMenu::addWidget(MenuWidget* widget)
{
    widget->initializeSize(mRenderer);
    
    mItems.AddTail(widget);

    // focus on first selection
    if (this->mCurrentSelection == -1)
    {
        this->mCurrentSelection = 0;
        this->mWidgetToRenderFrom = 0;
        widget->onGainFocus();
    }
}

void DebugMenu::addWidgetToNewestMenu(MenuWidget* widget)
{
    if(mNewestMenu)
        mNewestMenu->addWidget(widget);
}

void DebugMenu::endNewestMenu()
{
    if (mNewestMenu)
    {
        if (mNewestMenu->mParent)
            mNewestMenu = mNewestMenu->mParent;
    }
}

void DebugMenu::RemoveAllFocus()
{
    MenuWidget* widget = mItems.GetHead();

    while (widget)
    {
        widget->bHasFocus = false;
        widget = widget->GetNext();
    };

    return;
}

struct MenuWidget* DebugMenu::getWidget(int n)
{
    if (n < 0 || n >= mItems.CountElements()) // toru: added check for -1 to avoid crash
        return NULL;

    MenuWidget* widget = mItems.GetHead();

    for (int i = 0; i < n; i++)
    {
        widget = widget->GetNext();
    }

    return widget;
}

struct MenuWidget* DebugMenu::GetWidgetWithFocus()
{
    MenuWidget* widget = mItems.GetHead();
    
    while (widget)
    {
        if (widget->bHasFocus)
            return widget;
        else
            widget = widget->GetNext();
    };

    return NULL;
}

bool DebugMenu::selectionFitsOnScreen()
{
    if (this->mCurrentSelection == -1 || this->mWidgetToRenderFrom == -1)
        return true;

    int width = 0;
    int titleHeight = 0;
    int i = 0;
    int currentY = 0;
    char str[256] = { '\0' };

    // figure out title
    if (mCurrentMenu->mParent)
    {
        MenuWidget* widget = mCurrentMenu->mParent->GetWidgetWithFocus();

        if (widget)
        {
            sprintf(str, "[%s]", widget->name);
            titleHeight = widget->height;
        }
    }
    else
    {
        snprintf(str, sizeof(str), "[Build %s]", "Add version info here" );
        mRenderer->getStringSize(str, &width, &titleHeight);
    }
    
    currentY += titleHeight;

    MenuWidget* widget = mCurrentMenu->mItems.GetHead();

    while (true)
    {
        if (widget == mCurrentMenu->mItems.EndOfList() || widget->height + currentY > mCurrentMenu->mHeight)
            break;

        if (mCurrentSelection == i)
            return true;

        if (i >= mCurrentMenu->mWidgetToRenderFrom)
        {
            currentY += widget->getHeight();
        }

        i++;

        widget = widget->GetNext();
    }

    return false;
}

void DebugMenu::destroy()
{
    gLastCurMenuName = DebugMenu::mCurrentMenu->mName;
    gLastCurSelection = DebugMenu::mCurrentMenu->mCurrentSelection;

    mAllocations->DeleteAllElements();

    DebugMenu::mRootMenu = NULL;
    DebugMenu::mCurrentMenu = NULL;
    DebugMenu::mNewestMenu = NULL;
    
}

bool DebugMenu::isActive()
{
    return DebugMenu::mActive;
}

void DebugMenu::activate()
{
    DebugMenu::mActive = true;
}

void DebugMenu::deactivate()
{
    DebugMenu::mActive = false;
}

// rendering

void DebugMenu::doRender(IMenuRender* renderer)
{
    int currentY = 0;
    int i = 0;
    int width = 0;
    int titleHeight = 0;
    char subMenuTitle[256] = { '\0' };

    // figure out title
    if (mCurrentMenu->mParent)
    {
        MenuWidget* titleWidget = mCurrentMenu->mParent->GetWidgetWithFocus();

        if (titleWidget)
        {
            sprintf(subMenuTitle, "[%s]", titleWidget->name);
            titleHeight = titleWidget->height;
        }
    }
    else
    {
        snprintf(subMenuTitle, sizeof(subMenuTitle), "[Build %s]", "Add version info here");
        mRenderer->getStringSize(subMenuTitle, &width, &titleHeight);
    }

    currentY = titleHeight; // get its height

    // print it
    mRenderer->print(mCurrentMenu->mOriginX + 5, mCurrentMenu->mOriginY, 0xFF00FF00, subMenuTitle);

    MenuWidget* widget = mCurrentMenu->mItems.GetHead();

    while (true)
    {
        if (widget == mCurrentMenu->mItems.EndOfList() || widget->height + currentY > mCurrentMenu->mHeight)
            break;

        if (i >= mCurrentMenu->mWidgetToRenderFrom)
        {
            widget->render(mRenderer, mCurrentMenu->mOriginX + 20, mCurrentMenu->mOriginY + currentY);
            currentY += widget->getHeight();
        }

        i++;

        widget = widget->GetNext();
    }
}

void DebugMenu::doRenderBackground(IMenuRender* renderer) const
{
    mRenderer->renderOverlay(mOriginX - 5, mOriginY - 5, mWidth + 10, mHeight + 10, DebugMenuTransparencyLevel, 0xFFFF);
    mRenderer->renderOverlay(mOriginX, mOriginY, mWidth, mHeight, DebugMenuTransparencyLevel, mBackgroundColor);
}

void DebugMenu::render()
{
    static bool firstTime = true;

    if (mCurrentMenu && mActive)
    {
        DoScreenPrintf = false;
        mCurrentMenu->doRender(mRenderer);
    }

    if (firstTime)
    {
        firstTime = false;
        mSoakTicker = tGetTicker();
    }
    SoakTimer();
}

void DebugMenu::renderBackground()
{
    if (mRenderer && mCurrentMenu && mActive)
    {
        mCurrentMenu->doRenderBackground(mRenderer);
    }
}

// inputs

void DebugMenu::actionUp()
{
    if (mCurrentSelection > 0)
    {
        MenuWidget* widget = DebugMenu::getWidget(mCurrentSelection);
        widget->onLoseFocus();
    
        mCurrentSelection--;
    
        widget = DebugMenu::getWidget(mCurrentSelection);
        widget->onGainFocus();
    
        if (mWidgetToRenderFrom > mCurrentSelection)
            mWidgetToRenderFrom = mCurrentSelection;
    }
}

void DebugMenu::actionDown()
{
    int count = mItems.CountElements();
    if (mCurrentSelection < count - 1)
    {
        MenuWidget* widget = DebugMenu::getWidget(mCurrentSelection);
        widget->onLoseFocus();

        mCurrentSelection++;

        widget = DebugMenu::getWidget(mCurrentSelection);
        widget->onGainFocus();

        while (!selectionFitsOnScreen())
            ++mWidgetToRenderFrom;
    }
}

void DebugMenu::actionLeft()
{
    if (mCurrentSelection >= 0)
    {
        mCurrentMenu->getWidget(mCurrentMenu->mCurrentSelection)->onAction(MenuWidget::actions::ACTION_LEFT, 0);
    }
}

void DebugMenu::actionRight()
{
    if (mCurrentSelection >= 0)
    {
        mCurrentMenu->getWidget(mCurrentMenu->mCurrentSelection)->onAction(MenuWidget::actions::ACTION_RIGHT, 0);
    }
}

void DebugMenu::actionSelect()
{
    if (mCurrentSelection >= 0)
    {
        mCurrentMenu->getWidget(mCurrentMenu->mCurrentSelection)->onAction(MenuWidget::actions::ACTION_SELECT, 0);
    }
}

void DebugMenu::actionCancel()
{
    if (mCurrentMenu->mParent)
        mCurrentMenu = mCurrentMenu->mParent;
}

void DebugMenu::eventUp()
{
    if (mCurrentMenu)
        mCurrentMenu->actionUp();
}

void DebugMenu::eventDown()
{
    if (mCurrentMenu)
        mCurrentMenu->actionDown();
}

void DebugMenu::eventLeft()
{
    if (mCurrentMenu)
        mCurrentMenu->actionLeft();
}

void DebugMenu::eventRight()
{
    if (mCurrentMenu)
        mCurrentMenu->actionRight();
}

void DebugMenu::eventSelect()
{
    if (mCurrentMenu)
        mCurrentMenu->actionSelect();
}

void DebugMenu::eventCancel()
{
    if (mCurrentMenu)
        mCurrentMenu->actionCancel();
}

void DebugMenu::SetCurrentMenu()
{
    if (!mAllocations->IsEmpty())
    {
		DebugMenuAlloc* alloc = mAllocations->GetHead();
        // get the correct menu
        while (strcmp(alloc->menu->GetName(), gLastCurMenuName))
        {
            alloc = alloc->GetNext();
            if (alloc == mAllocations->EndOfList())
                break;
        }

        // get the correct widget
        mCurrentMenu = alloc->menu;
        MenuWidget *widget = mCurrentMenu->getWidget(gLastCurSelection);

        if (widget)
        {
            if (strcmp(mCurrentMenu->GetWidgetWithFocus()->getName(), widget->getName()))
            {
                while (strcmp(mCurrentMenu->GetWidgetWithFocus()->getName(), widget->getName()))
                {
                    mCurrentMenu->actionDown();
                }
            }
        }
    }
}

extern void ScreenPrintf(int x, int y, float duration, unsigned int color, char const* fmt, ...);

void DebugMenu::SoakTimer()
{
    static int ten_seconds = 0;
    static int minutes = 0;
    static int hours = 0;
    float ticker = tGetTickerDifference(mSoakTicker, tGetTicker()) * 0.001f;
    if (ticker >= 10.0)
    {
        if (ten_seconds == 5)
        {
            minutes++;
            if (minutes >= 60)
            {
                ++hours;
                minutes = 0;
            }
            ticker = 0.0;
            ten_seconds = 0;
        }
        else
        {
            ticker = 0.0;
            ++ten_seconds;
        }
        mSoakTicker = tGetTicker();
    }
    if (DebugMenu::mDoSoakTimer)
    {
        ScreenPrintf(0, -215, "Soak Timer");
        ScreenPrintf(0, -200, "%02d : %02d : %d%.3f", hours, minutes, ten_seconds, ticker);
    }
}
