/*
	*** Wizard Widget Page Info Functions
	*** src/gui/wizardwidgetpageinfo.cpp
	Copyright T. Youngs 2012-2018

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

// Constructor / Destructor
WizardWidgetPageInfo::WizardWidgetPageInfo() : ListItem<WizardWidgetPageInfo>()
{
	pageType_ = WizardWidgetPageInfo::NormalPage;
}

WizardWidgetPageInfo::~WizardWidgetPageInfo()
{
}

/*
 * Data
 */

// Set page information
void WizardWidgetPageInfo::set(int index, const char* title, int nextIndex)
{
	index_ = index;
	title_ = title;
	nextIndex_ = nextIndex;
}

// Set page type
void WizardWidgetPageInfo::setPageType(WizardWidgetPageInfo::PageType type)
{
	pageType_ = type;
}

// Return page type
WizardWidgetPageInfo::PageType WizardWidgetPageInfo::pageType() const
{
	return pageType_;
}

// Return identifying index for page
int WizardWidgetPageInfo::index() const
{
	return index_;
}

// Return title text for page
const char* WizardWidgetPageInfo::title() const
{
	return title_.get();
}

// Return index of the 'next' page
int WizardWidgetPageInfo::nextIndex() const
{
	return nextIndex_;
}

// Return whether this is an exit (finish) point
bool WizardWidgetPageInfo::isFinishPoint() const
{
	return nextIndex_ == -1;
}
