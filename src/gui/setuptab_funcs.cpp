/*
	*** SetupTab Functions
	*** src/gui/setuptab_funcs.cpp
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

#include "gui/setuptab.h"

// Constructor / Destructor
SetupTab::SetupTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);
}

SetupTab::~SetupTab()
{
}

/*
 * SubWidget / SubWindow Handling
 */

// Return the tab's SubWindow area, if it has one
QMdiArea* SetupTab::subWindowArea()
{
	return NULL;
}

// Return the tab's SubWidget layout, if it has one
QLayout* SetupTab::subWidgetLayout()
{
	return NULL;
}

/*
 * Update
 */

// Update controls in tab
void SetupTab::updateControls()
{
}

// Disable sensitive controls within tab, ready for main code to run
void SetupTab::disableSensitiveControls()
{
}

// Enable sensitive controls within tab, ready for main code to run
void SetupTab::enableSensitiveControls()
{
}

/*
 * State
 */

// Write widget state through specified LineParser
bool SetupTab::writeState(LineParser& parser)
{
	return true;
}
