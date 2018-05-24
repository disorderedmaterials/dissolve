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
#include <QToolButton>

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

// Add close button to specified tab
QToolButton* SuperTabWidget::addTabCloseButton(QWidget* pageWidget)
{
	// Find the tab containing the specified page
	int tabIndex = indexOf(pageWidget);
	if (tabIndex == -1)
	{
		Messenger::error("SuperTabWidget::setTabFont - Failed to find tab containing widget %p.\n", pageWidget);
		return NULL;
	}

	// Create a suitable tool button for the tab
	QToolButton* closeButton = new QToolButton;
	closeButton->setIcon(QIcon(":/general/icons/general_cross.svg"));
	closeButton->setIconSize(QSize(10,10));
	closeButton->setAutoRaise(true);
	superTabBar_->setTabButton(tabIndex, QTabBar::RightSide	, closeButton);
	connect(closeButton, SIGNAL(clicked(bool)), this, SLOT(tabCloseButtonClicked(bool)));
	closeButtons_.add(closeButton, pageWidget);

	return closeButton;
}

/*
 * Widget Functions
 */

// Tab close button clicked
void SuperTabWidget::tabCloseButtonClicked(bool checked)
{
	// Find the close button that sent the signal in our buttons reflist
	QToolButton* toolButton = (QToolButton*) sender();
	if (!toolButton) return;

	RefListItem<QToolButton,QWidget*>* item = closeButtons_.contains(toolButton);
	if (item)
	{
		// Find the tab containing the page widget (stored as the RefListItem's data)
		int tabIndex = indexOf(item->data);
		if (tabIndex == -1)
		{
			Messenger::error("SuperTabWidget::tabCloseButtonClicked - Failed to find tab containing widget %p.\n", item->data);
			return;
		}

		// Remove the button item
		closeButtons_.remove(item);

		// Signal that the specified page widget no longer exists. The main GUI will handle deletion of the page in the TabBar
		emit(tabClosed(item->data));
	}
	else printf("Tabs received a close event from an unknown button...\n");
}
