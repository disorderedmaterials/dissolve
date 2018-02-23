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
#include "gui/delegates/combolist.hui"
#include "gui/delegates/texponentialspin.hui"
#include "main/duq.h"
#include "classes/speciesbond.h"
#include "classes/speciesangle.h"
#include "classes/speciestorsion.h"
#include "templates/listwidgetupdater.h"
#include "templates/tablewidgetupdater.h"
#include <QListWidgetItem>

// Constructor / Destructor
SystemTab::SystemTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);

	// Set item delegates for tables
	// -- Functional Forms
	ui.MasterBondsTable->setItemDelegateForColumn(1, new ComboListDelegate(this, new ComboListEnumItems(SpeciesBond::nBondFunctions, SpeciesBond::bondFunctions())));

	// -- Parameters
	for (int n=2; n<6; ++n)
	{
		ui.MasterBondsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
		ui.MasterAnglesTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
		ui.MasterTorsionsTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
	}

	// Ensure fonts for table headers are set correctly
	ui.MasterBondsTable->horizontalHeader()->setFont(font());
	ui.MasterAnglesTable->horizontalHeader()->setFont(font());
	ui.MasterTorsionsTable->horizontalHeader()->setFont(font());
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
void SystemTab::updateBondsTableRow(int row, MasterIntra* masterBond, bool createItems)
{
	QTableWidgetItem* item;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
		ui.MasterBondsTable->setItem(row, 0, item);
	}
	else item = ui.MasterBondsTable->item(row, 0);
	item->setText(masterBond->name());

	// Functional Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
		ui.MasterBondsTable->setItem(row, 1, item);
	}
	else item = ui.MasterBondsTable->item(row, 1);
	item->setText(SpeciesBond::bondFunction( (SpeciesBond::BondFunction) masterBond->form()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterBond));
			ui.MasterBondsTable->setItem(row, n+2, item);
		}
		else item = ui.MasterBondsTable->item(row, n+2);
		item->setText(QString::number(masterBond->parameter(n)));
	}
}

// Row update function for AnglesTable
void SystemTab::updateAnglesTableRow(int row, MasterIntra* masterAngle, bool createItems)
{
	QTableWidgetItem* item;
	QComboBox* combo;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
		ui.MasterAnglesTable->setItem(row, 0, item);
	}
	else item = ui.MasterAnglesTable->item(row, 0);
	item->setText(masterAngle->name());

	// Functional Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
		ui.MasterAnglesTable->setItem(row, 1, item);
	}
	else item = ui.MasterAnglesTable->item(row, 1);
	item->setText(SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) masterAngle->form()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterAngle));
			ui.MasterAnglesTable->setItem(row, n+2, item);
		}
		else item = ui.MasterAnglesTable->item(row, n+2);
		item->setText(QString::number(masterAngle->parameter(n)));
	}
}

// Row update function for TorsionsTable
void SystemTab::updateTorsionsTableRow(int row, MasterIntra* masterTorsion, bool createItems)
{
	QTableWidgetItem* item;
	QComboBox* combo;

	// Name
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
		ui.MasterTorsionsTable->setItem(row, 0, item);
	}
	else item = ui.MasterTorsionsTable->item(row, 0);
	item->setText(masterTorsion->name());

	// Functional Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
		ui.MasterTorsionsTable->setItem(row, 1, item);
	}
	else item = ui.MasterTorsionsTable->item(row, 1);
	item->setText(SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) masterTorsion->form()));

	// Parameters
	for (int n=0; n<MAXINTRAPARAMS; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<MasterIntra>(masterTorsion));
			ui.MasterTorsionsTable->setItem(row, n+2, item);
		}
		else item = ui.MasterTorsionsTable->item(row, n+2);
		item->setText(QString::number(masterTorsion->parameter(n)));
	}
}

// Update controls in tab
void SystemTab::updateControls()
{
	refreshing_ = true;

	// Master Bonds Table
	TableWidgetUpdater<SystemTab,MasterIntra> bondsUpdater(ui.MasterBondsTable, duq_.masterBonds(), this, &SystemTab::updateBondsTableRow);

	// Master Angles Table
	TableWidgetUpdater<SystemTab,MasterIntra> anglesUpdater(ui.MasterAnglesTable, duq_.masterAngles(), this, &SystemTab::updateAnglesTableRow);

	// Torsions Table
	TableWidgetUpdater<SystemTab,MasterIntra> torsionsUpdater(ui.MasterTorsionsTable, duq_.masterTorsions(), this, &SystemTab::updateTorsionsTableRow);

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
