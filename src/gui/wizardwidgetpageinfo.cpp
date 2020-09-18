/*
    *** Wizard Widget Page Info Functions
    *** src/gui/wizardwidgetpageinfo.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/wizardwidgetpageinfo.h"

WizardWidgetPageInfo::WizardWidgetPageInfo() : ListItem<WizardWidgetPageInfo>()
{
    pageType_ = WizardWidgetPageInfo::NormalPage;
    stackedWidgetPage_ = nullptr;
}

WizardWidgetPageInfo::~WizardWidgetPageInfo() {}

/*
 * Data
 */

// Set page information
void WizardWidgetPageInfo::set(int index, const QString title, int nextIndex)
{
    index_ = index;
    title_ = title;
    nextIndex_ = nextIndex;
}

// Set page index
void WizardWidgetPageInfo::setIndex(int index) { index_ = index; }

// Set page title
void WizardWidgetPageInfo::setTitle(const QString title) { title_ = title; }

// Set next page index
void WizardWidgetPageInfo::setNextPageIndex(int nextIndex) { nextIndex_ = nextIndex; }

// Set page type
void WizardWidgetPageInfo::setPageType(WizardWidgetPageInfo::PageType type) { pageType_ = type; }

// Return page type
WizardWidgetPageInfo::PageType WizardWidgetPageInfo::pageType() const { return pageType_; }

// Return identifying index for page
int WizardWidgetPageInfo::index() const { return index_; }

// Return title text for page
const QString WizardWidgetPageInfo::title() const { return title_; }

// Return index of the 'next' page
int WizardWidgetPageInfo::nextIndex() const { return nextIndex_; }

// Return whether this is an exit (finish) point
bool WizardWidgetPageInfo::isFinishPoint() const { return nextIndex_ == -1; }

/*
 * Widget Interaction
 */

// Set Associated widget in QStackedWidget
void WizardWidgetPageInfo::setStackedWidgetPage(QWidget *widget) { stackedWidgetPage_ = widget; }

// Return associated widget in QStackedWidget
QWidget *WizardWidgetPageInfo::stackedWidgetPage() const { return stackedWidgetPage_; }

// Add new widget highlight
WizardWidgetHighlight *WizardWidgetPageInfo::addWidgetHighlight() { return widgetHighlights_.add(); }

// Highlight all defined widgets
void WizardWidgetPageInfo::highlightWidgets()
{
    ListIterator<WizardWidgetHighlight> highlightIterator(widgetHighlights_);
    while (WizardWidgetHighlight *wh = highlightIterator.iterate())
        wh->highlight();
}

// Un-highlight all defined widgets
void WizardWidgetPageInfo::unHighlightWidgets()
{
    ListIterator<WizardWidgetHighlight> highlightIterator(widgetHighlights_);
    while (WizardWidgetHighlight *wh = highlightIterator.iterate())
        wh->unHighlight();
}
