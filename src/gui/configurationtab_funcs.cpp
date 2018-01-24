/*
	*** ConfigurationTab Functions
	*** src/gui/configurationtab_funcs.cpp
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

#include "gui/configurationtab.h"

// Constructor / Destructor
ConfigurationTab::ConfigurationTab(DUQ& dUQ, QTabWidget* parent, const char* title) : MainTab(dUQ, parent, name)
{
}

ConfigurationTab::~ConfigurationTab()
{
}

/*
 * Data
 */

// Return MDI area available in tab (if any)
QMdiArea* ConfigurationTab::mdiArea()
{
	return ui.WorkspaceArea;
}

/*
 * Update
 */

// Update controls in page
void ConfigurationTab::updatePage()
{
}

// Disable sensitive controls within page, ready for main code to run
void ConfigurationTab::disableSensitiveControlsInPage()
{
}

// Enable sensitive controls within page, ready for main code to run
void ConfigurationTab::enableSensitiveControlsInPage()
{
}
