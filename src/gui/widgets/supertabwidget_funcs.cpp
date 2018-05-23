/*
	*** Super Tab Widget
	*** src/gui/widgets/supertabwidget_funcs.cpp
	Copyright T. Youngs 2013-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/widgets/supertabwidget.hui"
#include "gui/widgets/supertabbar.hui"
#include "base/messenger.h"

// Constructor
SuperTabWidget::SuperTabWidget(QWidget* parent) : QTabWidget(parent)
{
	// Create our own tab bar
	superTabBar_ = new SuperTabBar(this);
	setTabBar(superTabBar_);

}

/*
 * Tab Styling
 */

// Set text colour for tab with specified page widget
void SuperTabWidget::setTabTextColour(QWidget* pageWidget, QColor colour)
{
	// Find the tab containing the specified page
	int tabIndex = indexOf(pageWidget);
	if (tabIndex == -1)
	{
		Messenger::error("SuperTabWidget::setTabFont - Failed to find tab containing widget %p.\n", pageWidget);
		return;
	}

	// Set the style via the tab bar
	superTabBar_->setTabTextColor(tabIndex, colour);
}

// Set icon for tab with specified page widget
void SuperTabWidget::setTabIcon(QWidget* pageWidget, QIcon icon)
{
	// Find the tab containing the specified page
	int tabIndex = indexOf(pageWidget);
	if (tabIndex == -1)
	{
		Messenger::error("SuperTabWidget::setTabFont - Failed to find tab containing widget %p.\n", pageWidget);
		return;
	}

	// Set the style via the tab bar
	superTabBar_->setTabIcon(tabIndex, icon);
	
}
