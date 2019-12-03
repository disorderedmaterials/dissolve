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
#include "module/module.h"
#include "base/lineparser.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <QFrame>
#include <QInputDialog>
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

// Return enum options for TabType
EnumOptions<MainTab::TabType> MainTab::tabTypes()
{
	static EnumOptionsList TabTypeOptions = EnumOptionsList() <<
		EnumOption(MainTab::ConfigurationTabType, 	"ConfigurationTab") <<
		EnumOption(MainTab::ForcefieldTabType, 		"ForcefieldTab") <<
		EnumOption(MainTab::LayerTabType, 		"LayerTab") <<
		EnumOption(MainTab::ModuleTabType, 		"ModuleTab") <<
		EnumOption(MainTab::SpeciesTabType, 		"SpeciesTab") <<
		EnumOption(MainTab::WorkspaceTabType, 		"WorkspaceTab");

	static EnumOptions<MainTab::TabType> options("TabType", TabTypeOptions);

	return options;
}

/*
 * Data
 */

// Raise suitable dialog for entering / checking new tab name
QString MainTab::getNewTitle(bool& ok)
{
	return QInputDialog::getText(page_, "Rename Tab", "Enter the new name for the tab", QLineEdit::Normal, title_.get(), &ok);
}

// Return page widget
QWidget* MainTab::page() const
{
	return page_;
}

// Return title of tab
const char* MainTab::title() const
{
	return title_.get();
}

/*
 * Management
 */

// Return whether the title of the tab can be changed
bool MainTab::canChangeTitle() const
{
	return false;
}

// Rename tab through suitable dialog / widget
bool MainTab::rename()
{
	if (!canChangeTitle()) return false;

	// Find ourselves in the tab widget
	int tabIndex = tabWidget_->indexOf(page_);
	if (tabIndex == -1)
	{
		Messenger::print("Couldn't rename tab '%s' since its page widget could not be found.\n", title_.get());
		return false;
	}

	// Get the new name
	bool ok;
	QString text = getNewTitle(ok);
	if (!ok) return false;

	title_ = qPrintable(text);
	tabWidget_->setTabText(tabIndex, text);

	return true;
}

// Return whether the tab can be closed
bool MainTab::canClose() const
{
	return true;
}

// Close tab, deleting any necessary data
bool MainTab::close()
{
	return canClose();
}
