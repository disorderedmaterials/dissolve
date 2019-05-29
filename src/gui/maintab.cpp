/*
	*** Main Tab
	*** src/gui/maintab.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/maintab.h"
#include "gui/gui.h"
#include "gui/modulecontrolwidget.h"
#include "gui/widgets/subwindow.h"
#include "gui/widgets/subwidget.h"
#include "module/module.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QFrame>
#include <QLayout>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QTabWidget>

// Constructor / Destructor
MainTab::MainTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* tabWidget, const char* title, QWidget* page) : dissolve_(dissolve)
{
	dissolveWindow_ = dissolveWindow;
	tabWidget_ = tabWidget;
	page_ = page;
	title_ = title;
}

MainTab::~MainTab()
{
}

/*
 * Data
 */

// Return page widget
QWidget* MainTab::page() const
{
	return page_;
}

// Set title of tab
void MainTab::setTitle(const char* title)
{
	if (canChangeTitle())
	{
		// Find ourselves in the tab widget
		int index = tabWidget_->indexOf(page_);
		if (index == -1)
		{
			Messenger::print("Couldn't rename tab '%s' since its page widget could not be found.\n", title_.get());
			return;
		}
		title_ = title;
		tabWidget_->setTabText(index, title);
	}
	else Messenger::warn("Refusing to change title of tab currently named '%s', since it is fixed.\n", title_.get());
}

// Return title of tab
const char* MainTab::title() const
{
	return title_.get();
}

// Return whether the title of the tab can be changed
bool MainTab::canChangeTitle() const
{
	return false;
}
