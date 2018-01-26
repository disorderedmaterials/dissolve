/*
	*** Main Tab
	*** src/gui/maintab.cpp
	Copyright T. Youngs 2012-2018

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

#include "gui/maintab.h"
#include "gui/subwindow.h"
#include "gui/subwidget.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QTabWidget>

// Constructor / Destructor
MainTab::MainTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* tabWidget, const char* title, QWidget* page) : ListItem<MainTab>(), duq_(duq)
{
	duqWindow_ = duqWindow;
	tabWidget_ = tabWidget;
	title_ = title;

	// Add tab to the parent tabwidget
	if (tabWidget_) tabWidget_->addTab(page, title_.get());
}

MainTab::~MainTab()
{
	// Delete the tab from the tabwidget first - find its index (based on our, the page's, pointer) and remove that
	int index = tabWidget_->indexOf( (QWidget*) this);
	if (index == -1) Messenger::print("Couldn't remove tab '%s' since its page widget could not be found.\n", title_.get());
	else
	{
		tabWidget_->removeTab(index);
	}
}

/*
 * Data
 */

// Return title of tab
const char* MainTab::title() const
{
	return title_.get();
}
