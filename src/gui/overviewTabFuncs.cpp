#include "gui/overviewTab.h"


OverviewTab::OverviewTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title) : MainTab(dissolveWindow, dissolve, parent, title, this) {}

MainTab::TabType OverviewTab::type() const
{
    return MainTab::TabType::Overview;
}

void OverviewTab::updateControls()
{

}

void OverviewTab::preventEditing()
{

}

void OverviewTab::allowEditing()
{

}