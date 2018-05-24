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
#include "gui/delegates/intraformcombo.hui"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/integerspin.hui"
#include "gui/delegates/isotopecombo.hui"
#include "gui/delegates/texponentialspin.hui"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/duq.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/speciesbond.h"
#include <QListWidgetItem>

// Constructor / Destructor
SpeciesTab::SpeciesTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title) : MainTab(duqWindow, duq, parent, title, this)
{
	ui.setupUi(this);

	refreshing_ = true;

	// Set item delegates in tables
	// -- SpeciesAtomTable
	for (int n=1; n<5; ++n) ui.AtomTable->setItemDelegateForColumn(n, new TExponentialSpinDelegate(this));
	ui.AtomTable->setItemDelegateForColumn(5, new ComboListDelegate(this, new ComboNameListItems<AtomType>(duq_.atomTypeList())));
	ui.AtomTable->horizontalHeader()->setFont(font());
	// -- Isotope Table
	ui.IsotopeTable->setItemDelegateForColumn(1, new IsotopeComboDelegate(this));
	// -- Bond Table
	for (int n=0; n<2; ++n) ui.BondTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	ui.BondTable->setItemDelegateForColumn(2, new IntraFormComboDelegate(this, SpeciesBond::nBondFunctions, SpeciesBond::bondFunctions(), duq_.masterBonds()));
	// -- Angle Table
	for (int n=0; n<3; ++n) ui.AngleTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	// -- Torsion Table
	for (int n=0; n<4; ++n) ui.TorsionTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));

	refreshing_ = false;
}

SpeciesTab::~SpeciesTab()
{
}

/*
 * Update
 */


// SpeciesAtomTable row update function
void SpeciesTab::updateAtomTableRow(int row, SpeciesAtom* speciesAtom, bool createItems)
{
	QTableWidgetItem* item;

	// Element
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui.AtomTable->setItem(row, 0, item);
	}
	else item = ui.AtomTable->item(row, 0);
	item->setText(PeriodicTable::element(speciesAtom->element()).name());

	// Coordinates
	for (int n=0; n<3; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
			ui.AtomTable->setItem(row, n+1, item);
		}
		else item = ui.AtomTable->item(row, n+1);
		item->setText(QString::number(speciesAtom->r().get(n)));
	}

	// Charge
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui.AtomTable->setItem(row, 4, item);
	}
	else item = ui.AtomTable->item(row, 4);
	item->setText(QString::number(speciesAtom->charge()));

	// AtomTypes
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui.AtomTable->setItem(row, 5, item);
	}
	else item = ui.AtomTable->item(row, 5);
	item->setText(speciesAtom->atomType() ? speciesAtom->atomType()->name() : "");
}

// IsotopologuesIsotopesTable row update function
void SpeciesTab::updateIsotopeTableRow(int row, AtomType* atomType, Isotope* isotope, bool createItems)
{
	QTableWidgetItem* item;

	// AtomType
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<AtomType>(atomType));
		item->setFlags(Qt::NoItemFlags);
		ui.IsotopeTable->setItem(row, 0, item);
	}
	else item = ui.IsotopeTable->item(row, 0);
	item->setText(atomType->name());

	// Isotope
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<Isotope>(isotope));
		ui.IsotopeTable->setItem(row, 1, item);
	}
	else item = ui.IsotopeTable->item(row, 1);
	item->setText(IsotopeComboDelegate::textForIsotope(isotope));
}

// BondTable row update function
void SpeciesTab::updateBondTableRow(int row, SpeciesBond* speciesBond, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<2; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
			ui.BondTable->setItem(row, n, item);
		}
		else item = ui.BondTable->item(row, n);
		item->setText(QString::number(speciesBond->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
		ui.BondTable->setItem(row, 2, item);
	}
	else item = ui.BondTable->item(row, 2);
	item->setText(speciesBond->masterParameters() ? QString("@%1").arg(speciesBond->masterParameters()->name()) : SpeciesBond::bondFunction( (SpeciesBond::BondFunction) speciesBond->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
			ui.BondTable->setItem(row, n+3, item);
		}
		else item = ui.BondTable->item(row, n+3);
		item->setText(QString::number(speciesBond->parameter(n)));
		item->setFlags(speciesBond->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

// AngleTable row update function
void SpeciesTab::updateAngleTableRow(int row, SpeciesAngle* speciesAngle, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<3; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
			ui.AngleTable->setItem(row, n, item);
		}
		else item = ui.AngleTable->item(row, n);
		item->setText(QString::number(speciesAngle->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
		ui.AngleTable->setItem(row, 3, item);
	}
	else item = ui.AngleTable->item(row, 3);
	item->setText(speciesAngle->masterParameters() ? QString("@%1").arg(speciesAngle->masterParameters()->name()) : SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) speciesAngle->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
			ui.AngleTable->setItem(row, n+4, item);
		}
		else item = ui.AngleTable->item(row, n+4);
		item->setText(QString::number(speciesAngle->parameter(n)));
		item->setFlags(speciesAngle->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

// TorsionTable row update function
void SpeciesTab::updateTorsionTableRow(int row, SpeciesTorsion* speciesTorsion, bool createItems)
{
	QTableWidgetItem* item;

	// Atom Indices
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
			ui.TorsionTable->setItem(row, n, item);
		}
		else item = ui.TorsionTable->item(row, n);
		item->setText(QString::number(speciesTorsion->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
		ui.TorsionTable->setItem(row, 4, item);
	}
	else item = ui.TorsionTable->item(row, 4);
	item->setText(speciesTorsion->masterParameters() ? QString("@%1").arg(speciesTorsion->masterParameters()->name()) : SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) speciesTorsion->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
			ui.TorsionTable->setItem(row, n+5, item);
		}
		else item = ui.TorsionTable->item(row, n+5);
		item->setText(QString::number(speciesTorsion->parameter(n)));
		item->setFlags(speciesTorsion->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
}

// Update controls in tab
void SpeciesTab::updateControls()
{
	refreshing_ = true;

	// Species List
	ListWidgetUpdater<SpeciesTab,Species> speciesUpdater(ui.SpeciesList, duq_.species());

	Species* species = currentSpecies();

	// SpeciesAtom Table
	if (!species) ui.AtomTable->clearContents();
	else TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesAtomUpdater(ui.AtomTable, species->atoms(), this, &SpeciesTab::updateAtomTableRow);

	// Isotopologues List
	if (!species) ui.IsotopologueList->clear();
	else ListWidgetUpdater<SpeciesTab,Isotopologue> isotopologueUpdater(ui.IsotopologueList, species->isotopologues());

	Isotopologue* isotopologue = currentIsotopologue();

	// Isotopologue AtomType/Isotopes Table
	if (!isotopologue) ui.IsotopeTable->clearContents();
	else TableWidgetRefListUpdater<SpeciesTab,AtomType,Isotope*> isotopeUpdater(ui.IsotopeTable, isotopologue->isotopes(), this, &SpeciesTab::updateIsotopeTableRow);
	ui.IsotopeTable->resizeColumnsToContents();

	// Intramolecular terms
	if (!species)
	{
		ui.BondTable->clearContents();
		ui.AngleTable->clearContents();
		ui.TorsionTable->clearContents();
	}
	else
	{
		TableWidgetUpdater<SpeciesTab,SpeciesBond> bondUpdater(ui.BondTable, species->bonds(), this, &SpeciesTab::updateBondTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesAngle> angleUpdater(ui.AngleTable, species->angles(), this, &SpeciesTab::updateAngleTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesTorsion> torsionUpdater(ui.TorsionTable, species->torsions(), this, &SpeciesTab::updateTorsionTableRow);
	}
	ui.BondTable->resizeColumnsToContents();
	ui.AngleTable->resizeColumnsToContents();
	ui.TorsionTable->resizeColumnsToContents();

	refreshing_ = false;

	// If no current Species it selected, set one now
	if ((ui.SpeciesList->currentRow() == -1) && (ui.SpeciesList->count() > 0)) ui.SpeciesList->setCurrentRow(0);
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

// Return currently-selected Isotopologue
Isotopologue* SpeciesTab::currentIsotopologue()
{
	QListWidgetItem* item = ui.IsotopologueList->currentItem();
	if (!item) return NULL;
	return (Isotopologue*) VariantPointer<Isotopologue>(item->data(Qt::UserRole));
}

void SpeciesTab::on_SpeciesList_currentRowChanged(int row)
{
	if (refreshing_) return;

	// Clear relevant tables here to make things more efficient
	ui.AtomTable->clearContents();
	ui.IsotopologueList->clear();
	ui.IsotopeTable->clearContents();

	updateControls();
}

void SpeciesTab::on_AtomTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target SpeciesAtom from the passed widget
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
		// Charge
		case (4):
			speciesAtom->setCharge(w->text().toDouble());
			duqWindow_->setModified();
			break;
		// AtomType
		case (5):
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

void SpeciesTab::on_IsotopologueList_currentRowChanged(int row)
{
	if (refreshing_) return;

	Isotopologue* isotopologue = currentIsotopologue();

	refreshing_ = true;

	// Isotopologue AtomType/Isotopes Table
	if (!isotopologue) ui.IsotopeTable->clearContents();
	else TableWidgetRefListUpdater<SpeciesTab,AtomType,Isotope*> isotopeUpdater(ui.IsotopeTable, isotopologue->isotopes(), this, &SpeciesTab::updateIsotopeTableRow);

	refreshing_ = false;
}

void SpeciesTab::on_IsotopeTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get current Isotopologue
	Isotopologue* iso = currentIsotopologue();
	if (!iso) return;

	// Get row from the passed widget, and get target AtomType from column 0
	QTableWidgetItem* item = ui.IsotopeTable->item(w->row(), 0);
	if (!item) return;
	
	AtomType* atomType = VariantPointer<AtomType>(item->data(Qt::UserRole));
	if (!atomType) return;

	// Column of passed item tells us the type of data we need to change
	Isotope* isotope;
	switch (w->column())
	{
		// Element
		case (0):
			// Should never be called
			break;
		// Isotope
		case (1):
			// The new Isotope should have been set in the model data for the column
			isotope = VariantPointer<Isotope>(w->data(Qt::UserRole));
			if (isotope) iso->setAtomTypeIsotope(atomType, isotope);
			duqWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Isotopes table.\n", w->column());
			break;
	}
}

void SpeciesTab::on_BondTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get current Species
	Species* species = currentSpecies();
	if (!species) return;

	// Get target SpeciesBond from the passed widget
	SpeciesBond* speciesBond = w ? VariantPointer<SpeciesBond>(w->data(Qt::UserRole)) : NULL;
	if (!speciesBond) return;

	// Column of passed item tells us the type of data we need to change
	int i, j;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
			// Get both atom indices and set the atoms in the interaction
			i = ui.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui.BondTable->item(w->row(), 1)->text().toInt() - 1;
			if (species->reconnectBond(speciesBond, i, j))
			{
				updateRow = true;
				duqWindow_->setModified();
			}
			break;
		// Functional Form
		case (2):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = duq_.hasMasterBond(qPrintable(w->text()));
				speciesBond->setMasterParameters(master);
			}
			else
			{
				SpeciesBond::BondFunction bf = SpeciesBond::bondFunction(qPrintable(w->text()));
				speciesBond->setMasterParameters(NULL);
				speciesBond->setForm(bf);
			}
			updateRow = true;
			duqWindow_->setModified();
			break;
		// Parameters
		case (3):
		case (4):
		case (5):
		case (6):
			if (speciesBond->masterParameters()) Messenger::error("Refusing to set bond parameter since it belongs to a master term.\n");
			else   speciesBond->setParameter(w->column()-3, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Bond table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		refreshing_ = true;
		updateBondTableRow(w->row(), speciesBond, false);
		refreshing_ = false;
	}
}

void SpeciesTab::on_AngleTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get current Species
	Species* species = currentSpecies();
	if (!species) return;

	// Get target SpeciesAngle from the passed widget
	SpeciesAngle* speciesAngle = w ? VariantPointer<SpeciesAngle>(w->data(Qt::UserRole)) : NULL;
	if (!speciesAngle) return;

	// Column of passed item tells us the type of data we need to change
	int i, j, k;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
		case (2):
			// Get all atom indices and set the atoms in the interaction
			i = ui.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui.BondTable->item(w->row(), 2)->text().toInt() - 1;
			if (species->reconnectAngle(speciesAngle, i, j, k))
			{
				updateRow = true;
				duqWindow_->setModified();
			}
			break;
		// Functional Form
		case (3):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = duq_.hasMasterAngle(qPrintable(w->text()));
				speciesAngle->setMasterParameters(master);
			}
			else
			{
				SpeciesAngle::AngleFunction af = SpeciesAngle::angleFunction(qPrintable(w->text()));
				speciesAngle->setMasterParameters(NULL);
				speciesAngle->setForm(af);
			}
			updateRow = true;
			duqWindow_->setModified();
			break;
		// Parameters
		case (4):
		case (5):
		case (6):
		case (7):
			if (speciesAngle->masterParameters()) Messenger::error("Refusing to set angle parameter since it belongs to a master term.\n");
			else   speciesAngle->setParameter(w->column()-4, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Angle table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		refreshing_ = true;
		updateAngleTableRow(w->row(), speciesAngle, false);
		refreshing_ = false;
	}
}

void SpeciesTab::on_TorsionTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get current Species
	Species* species = currentSpecies();
	if (!species) return;

	// Get target SpeciesTorsion from the passed widget
	SpeciesTorsion* speciesTorsion = w ? VariantPointer<SpeciesTorsion>(w->data(Qt::UserRole)) : NULL;
	if (!speciesTorsion) return;

	// Column of passed item tells us the type of data we need to change
	int i, j, k, l;
	bool updateRow = false;
	switch (w->column())
	{
		// Atom Indices
		case (0):
		case (1):
		case (2):
		case (3):
			// Get all atom indices and set the atoms in the interaction
			i = ui.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui.BondTable->item(w->row(), 2)->text().toInt() - 1;
			l = ui.BondTable->item(w->row(), 3)->text().toInt() - 1;
			if (species->reconnectTorsion(speciesTorsion, i, j, k, l))
			{
				updateRow = true;
				duqWindow_->setModified();
			}
			break;
		// Functional Form
		case (4):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = duq_.hasMasterTorsion(qPrintable(w->text()));
				speciesTorsion->setMasterParameters(master);
			}
			else
			{
				SpeciesTorsion::TorsionFunction tf = SpeciesTorsion::torsionFunction(qPrintable(w->text()));
				speciesTorsion->setMasterParameters(NULL);
				speciesTorsion->setForm(tf);
			}
			updateRow = true;
			duqWindow_->setModified();
			break;
		// Parameters
		case (5):
		case (6):
		case (7):
		case (8):
			if (speciesTorsion->masterParameters()) Messenger::error("Refusing to set torsion parameter since it belongs to a master term.\n");
			else   speciesTorsion->setParameter(w->column()-5, w->text().toDouble());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Torsion table.\n", w->column());
			break;
	}

	// Update the row if necessary
	if (updateRow)
	{
		refreshing_ = true;
		updateTorsionTableRow(w->row(), speciesTorsion, false);
		refreshing_ = false;
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
