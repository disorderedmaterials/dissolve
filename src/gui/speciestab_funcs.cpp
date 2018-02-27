/*
	*** SpeciesTab Functions
	*** src/gui/speciestab_funcs.cpp
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

#include "gui/speciestab.h"
#include "gui/gui.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/texponentialspin.hui"
#include "main/duq.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "templates/listwidgetupdater.h"
#include "templates/tablewidgetupdater.h"
#include <QListWidgetItem>

// Constructor / Destructor
SpeciesTab::SpeciesTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);

	// SpeciesAtomTable
	for (int n=1; n<4; ++n) ui.SpeciesAtomTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
	ui.SpeciesAtomTable->setItemDelegateForColumn(4, new ComboListDelegate(this, new ComboNameListItems<AtomType>(duq_.atomTypeList())));
	ui.SpeciesAtomTable->setHorizontalHeaderLabels(QStringList() << "E" << "X" << "Y" << "Z" << "AtomType");
	ui.SpeciesAtomTable->horizontalHeader()->setFont(duqWindow->font());
}

SpeciesTab::~SpeciesTab()
{
}

/*
 * SubWidget / SubWindow Handling
 */

// Return the tab's SubWindow area, if it has one
QMdiArea* SpeciesTab::subWindowArea()
{
	return NULL;
}

// Return the tab's SubWidget layout, if it has one
QLayout* SpeciesTab::subWidgetLayout()
{
	return NULL;
}

/*
 * Update
 */

// Update controls in tab
void SpeciesTab::updateControls()
{
	refreshing_ = true;

	ListWidgetUpdater<SpeciesTab,Species> speciesUpdater(ui.SpeciesList, duq_.species(), this, &SpeciesTab::updateSpeciesListRow);

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void SpeciesTab::disableSensitiveControls()
{
}

// Enable sensitive controls within tab, ready for main code to run
void SpeciesTab::enableSensitiveControls()
{
}

/*
 * Signals / Slots
 */

// Return currently-selected Species
Species* SpeciesTab::currentSpecies()
{
	QListWidgetItem* item = ui.SpeciesList->currentItem();
	if (!item) return NULL;
	return (Species*) VariantPointer<Species>(item->data(Qt::UserRole));
}

// SpeciesList row update function
void SpeciesTab::updateSpeciesListRow(int row, Species* species, bool createItem)
{
	QListWidgetItem* item;

	if (createItem)
	{
		item = new QListWidgetItem;
		ui.SpeciesList->addItem(item);
		item->setData(Qt::UserRole, VariantPointer<Species>(species));
	}
	else item = ui.SpeciesList->item(row);

	item->setText(species->name());
}

// SpeciesAtomTable row update function
void SpeciesTab::updateSpeciesAtomTableRow(int row, SpeciesAtom* speciesAtom, bool createItems)
{
	QTableWidgetItem* item;

	// Element
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui.SpeciesAtomTable->setItem(row, 0, item);
	}
	else item = ui.SpeciesAtomTable->item(row, 0);
	item->setText(PeriodicTable::element(speciesAtom->element()).name());

	// Coordinates
	for (int n=0; n<3; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
			ui.SpeciesAtomTable->setItem(row, n+1, item);
		}
		else item = ui.SpeciesAtomTable->item(row, n+2);
		item->setText(QString::number(speciesAtom->r().get(n)));
	}

	// AtomTypes
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui.SpeciesAtomTable->setItem(row, 4, item);
	}
	else item = ui.SpeciesAtomTable->item(row, 4);
	item->setText(speciesAtom->atomType() ? speciesAtom->atomType()->name() : "");

}

void SpeciesTab::on_SpeciesList_currentRowChanged(int row)
{
	ui.SpeciesAtomTable->clear();

	Species* species = currentSpecies();
	if (!species) return;

	// Update data
// 	ui.SpeciesAtomTable->setColumnCount();
	TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesUpdater(ui.SpeciesAtomTable, species->atoms(), this, &SpeciesTab::updateSpeciesAtomTableRow);	
}

void SpeciesTab::on_SpeciesAtomTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target MasterIntra from the passed widget
	SpeciesAtom* speciesAtom = w ? VariantPointer<SpeciesAtom>(w->data(Qt::UserRole)) : NULL;
	if (!speciesAtom) return;

	// Column of passed item tells us the type of data we need to change
	AtomType* atomType;
	switch (w->column())
	{
		// Element
		case (0):
			break;
		// Coordinates
		case (1):
		case (2):
		case (3):
			speciesAtom->setCoordinate(w->column()-1, w->text().toDouble());
			duqWindow_->setModified();
			break;
		// AtomType
		case (4):
			// Check the text to see if it is an existing AtomType - if not, we should create it
			atomType = duq_.findAtomType(qPrintable(w->text()));
			if (!atomType)
			{
				atomType = duq_.addAtomType(speciesAtom->element());
				atomType->setName(qPrintable(w->text()));
			}
			speciesAtom->setAtomType(atomType);
			duqWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of SpeciesAtom table.\n", w->column());
			break;
	}
}

/*
 * State
 */

// Write widget state through specified LineParser
bool SpeciesTab::writeState(LineParser& parser)
{
	return true;
}
