/*
	*** SystemTab Functions
	*** src/gui/systemtab_funcs.cpp
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

#include "gui/systemtab.h"
#include "gui/gui.h"
//#include "gui/delegates/combolist.hui"
//#include "gui/delegates/texponentialspin.hui"
//#include "main/duq.h"
//#include "templates/listwidgetupdater.h"
//#include "templates/tablewidgetupdater.h"
//#include <QListWidgetItem>

// Constructor / Destructor
SystemTab::SystemTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);

	refreshing_ = true;

	refreshing_ = false;
}

SystemTab::~SystemTab()
{
}

/*
 * SubWidget / SubWindow Handling
 */

// Return the tab's SubWindow area, if it has one
QMdiArea* SystemTab::subWindowArea()
{
	return NULL;
}

// Return the tab's SubWidget layout, if it has one
QLayout* SystemTab::subWidgetLayout()
{
	return NULL;
}

/*
 * Update
 */

// Row update function for BondsTable
//void SystemTab::updateBondsTableRow(int row, MasterIntra* masterBond, bool createItems)
//{
//}

// Update controls in tab
void SystemTab::updateControls()
{
	refreshing_ = true;

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void SystemTab::disableSensitiveControls()
{
}

// Enable sensitive controls within tab, ready for main code to run
void SystemTab::enableSensitiveControls()
{
}

/*
 * Signals / Slots
 */


/*
 * State
 */

// Write widget state through specified LineParser
bool SystemTab::writeState(LineParser& parser)
{
	return true;
}
