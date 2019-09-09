/*
	*** SpeciesTab Functions
	*** src/gui/speciestab_funcs.cpp
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

#include "gui/speciestab.h"
#include "gui/gui.h"
#include "gui/delegates/intraformcombo.hui"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/integerspin.hui"
#include "gui/delegates/isotopecombo.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/speciesbond.h"
#include <QListWidgetItem>

// Constructor / Destructor
SpeciesTab::SpeciesTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, Species* species) : ListItem<SpeciesTab>(), MainTab(dissolveWindow, dissolve, parent, CharString("Species: %s", title), this)
{
	ui_.setupUi(this);

	refreshing_ = true;

	species_ = species;

	// Set item delegates in tables
	// -- SpeciesAtomTable
	ui_.AtomTable->setItemDelegateForColumn(1, new ComboListDelegate(this, new ComboNameListItems<AtomType>(dissolve_.atomTypes())));
	for (int n=2; n<6; ++n) ui_.AtomTable->setItemDelegateForColumn(n, new ExponentialSpinDelegate(this));
	ui_.AtomTable->horizontalHeader()->setFont(font());
	// -- Bond Table
	for (int n=0; n<2; ++n) ui_.BondTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	ui_.BondTable->setItemDelegateForColumn(2, new IntraFormComboDelegate(this, SpeciesBond::nBondFunctions, SpeciesBond::bondFunctions(), dissolve_.coreData().masterBonds()));
	// -- Angle Table
	for (int n=0; n<3; ++n) ui_.AngleTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	// -- Torsion Table
	for (int n=0; n<4; ++n) ui_.TorsionTable->setItemDelegateForColumn(n, new IntegerSpinDelegate(this, 1, 1e9));
	// -- Isotope Table
	ui_.IsotopeTable->setItemDelegateForColumn(1, new IsotopeComboDelegate(this));

	// Set target for SpeciesViewer
	ui_.ViewerWidget->speciesViewer()->setSpecies(species_);

	// Connect signals / slots
	connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(dataChanged()), this, SLOT(updateControls()));
	connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(dataModified(bool)), dissolveWindow_, SLOT(setModified(bool)));

	refreshing_ = false;
}

SpeciesTab::~SpeciesTab()
{
}

/*
 * Data
 */

// Return tab type
const char* SpeciesTab::tabType() const
{
	return "SpeciesTab";
}

/*
 * Species Target
 */

// Return displayed Species
const Species* SpeciesTab::species() const
{
	return species_;
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
		ui_.AtomTable->setItem(row, 0, item);
	}
	else item = ui_.AtomTable->item(row, 0);
	item->setText(speciesAtom->element()->name());

	// AtomType
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui_.AtomTable->setItem(row, 1, item);
	}
	else item = ui_.AtomTable->item(row, 1);
	item->setText(speciesAtom->atomType() ? speciesAtom->atomType()->name() : "");

	// Coordinates
	for (int n=0; n<3; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
			ui_.AtomTable->setItem(row, n+2, item);
		}
		else item = ui_.AtomTable->item(row, n+2);
		item->setText(QString::number(speciesAtom->r().get(n)));
	}

	// Charge
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAtom>(speciesAtom));
		ui_.AtomTable->setItem(row, 5, item);
	}
	else item = ui_.AtomTable->item(row, 5);
	item->setText(QString::number(speciesAtom->charge()));
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
			ui_.BondTable->setItem(row, n, item);
		}
		else item = ui_.BondTable->item(row, n);
		item->setText(QString::number(speciesBond->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
		ui_.BondTable->setItem(row, 2, item);
	}
	else item = ui_.BondTable->item(row, 2);
	item->setText(speciesBond->masterParameters() ? QString("@%1").arg(speciesBond->masterParameters()->name()) : SpeciesBond::bondFunction( (SpeciesBond::BondFunction) speciesBond->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesBond>(speciesBond));
			ui_.BondTable->setItem(row, n+3, item);
		}
		else item = ui_.BondTable->item(row, n+3);
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
			ui_.AngleTable->setItem(row, n, item);
		}
		else item = ui_.AngleTable->item(row, n);
		item->setText(QString::number(speciesAngle->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
		ui_.AngleTable->setItem(row, 3, item);
	}
	else item = ui_.AngleTable->item(row, 3);
	item->setText(speciesAngle->masterParameters() ? QString("@%1").arg(speciesAngle->masterParameters()->name()) : SpeciesAngle::angleFunction( (SpeciesAngle::AngleFunction) speciesAngle->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesAngle>(speciesAngle));
			ui_.AngleTable->setItem(row, n+4, item);
		}
		else item = ui_.AngleTable->item(row, n+4);
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
			ui_.TorsionTable->setItem(row, n, item);
		}
		else item = ui_.TorsionTable->item(row, n);
		item->setText(QString::number(speciesTorsion->index(n)+1));
	}

	// Interaction Form
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
		ui_.TorsionTable->setItem(row, 4, item);
	}
	else item = ui_.TorsionTable->item(row, 4);
	item->setText(speciesTorsion->masterParameters() ? QString("@%1").arg(speciesTorsion->masterParameters()->name()) : SpeciesTorsion::torsionFunction( (SpeciesTorsion::TorsionFunction) speciesTorsion->form()));

	// Interaction Parameters
	for (int n=0; n<4; ++n)
	{
		if (createItems)
		{
			item = new QTableWidgetItem;
			item->setData(Qt::UserRole, VariantPointer<SpeciesTorsion>(speciesTorsion));
			ui_.TorsionTable->setItem(row, n+5, item);
		}
		else item = ui_.TorsionTable->item(row, n+5);
		item->setText(QString::number(speciesTorsion->parameter(n)));
		item->setFlags(speciesTorsion->masterParameters() ? Qt::NoItemFlags : Qt::ItemIsEnabled | Qt::ItemIsEditable );
	}
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
		ui_.IsotopeTable->setItem(row, 0, item);
	}
	else item = ui_.IsotopeTable->item(row, 0);
	item->setText(atomType->name());

	// Isotope
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<Isotope>(isotope));
		ui_.IsotopeTable->setItem(row, 1, item);
	}
	else item = ui_.IsotopeTable->item(row, 1);
	item->setText(IsotopeComboDelegate::textForIsotope(isotope));
}

// Update controls in tab
void SpeciesTab::updateControls()
{
	refreshing_ = true;

	// Definition
	ui_.NameEdit->setText(species_->name());

	// Isotopologues List
	if (!species_) ui_.IsotopologueList->clear();
	else
	{
		ListWidgetUpdater<SpeciesTab,Isotopologue> isotopologueUpdater(ui_.IsotopologueList, species_->isotopologues(), Qt::ItemIsEditable);

		// If there is no current isotopologue selected, try to select the first
		if (!currentIsotopologue()) ui_.IsotopologueList->setCurrentRow(0);
	}

	Isotopologue* isotopologue = currentIsotopologue();
	ui_.IsotopologueRemoveButton->setEnabled(isotopologue != NULL);

	// Isotopologue AtomType/Isotopes Table
	if (!isotopologue) ui_.IsotopeTable->clearContents();
	else TableWidgetRefDataListUpdater<SpeciesTab,AtomType,Isotope*> isotopeUpdater(ui_.IsotopeTable, isotopologue->isotopes(), this, &SpeciesTab::updateIsotopeTableRow);
	ui_.IsotopeTable->resizeColumnsToContents();

	// -- View / Generate Tab
	ui_.ViewerWidget->speciesViewer()->postRedisplay();

	// Geometry Tab

	// -- SpeciesAtom Table
	if (!species_) ui_.AtomTable->clearContents();
	else TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this, &SpeciesTab::updateAtomTableRow);

	// -- Intramolecular terms
	if (!species_)
	{
		ui_.BondTable->clearContents();
		ui_.AngleTable->clearContents();
		ui_.TorsionTable->clearContents();
	}
	else
	{
		TableWidgetUpdater<SpeciesTab,SpeciesBond> bondUpdater(ui_.BondTable, species_->bonds(), this, &SpeciesTab::updateBondTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesAngle> angleUpdater(ui_.AngleTable, species_->angles(), this, &SpeciesTab::updateAngleTableRow);
		TableWidgetUpdater<SpeciesTab,SpeciesTorsion> torsionUpdater(ui_.TorsionTable, species_->torsions(), this, &SpeciesTab::updateTorsionTableRow);
	}
	ui_.BondTable->resizeColumnsToContents();
	ui_.AngleTable->resizeColumnsToContents();
	ui_.TorsionTable->resizeColumnsToContents();

	refreshing_ = false;
}

// Disable sensitive controls within tab
void SpeciesTab::disableSensitiveControls()
{
	setEnabled(false);
}

// Enable sensitive controls within tab
void SpeciesTab::enableSensitiveControls()
{
	setEnabled(true);
}

/*
 * Signals / Slots
 */

// Return currently-selected Isotopologue
Isotopologue* SpeciesTab::currentIsotopologue()
{
	QListWidgetItem* item = ui_.IsotopologueList->currentItem();
	if (!item) return NULL;
	return VariantPointer<Isotopologue>(item->data(Qt::UserRole));
}

void SpeciesTab::on_NameEdit_textChanged(QString text)
{
	if (refreshing_) return;

	species_->setName(qPrintable(text));

	dissolveWindow_->setModified();
}

void SpeciesTab::on_IsotopologueAddButton_clicked(bool checked)
{
	species_->addIsotopologue("NewIsotopologue", dissolveWindow_->dissolve().atomTypes());

	updateControls();
}

void SpeciesTab::on_IsotopologueRemoveButton_clicked(bool checked)
{
	// Get current Isotopologue
	Isotopologue* iso = currentIsotopologue();
	if (!iso) return;

	// Notify all keywords that our Isotopologue is about to be removed
	KeywordBase::objectNoLongerValid<Isotopologue>(iso);

	// Finally, remove the Isotopologue from the Species
	species_->removeIsotopologue(iso);

	updateControls();
}

void SpeciesTab::on_IsotopologueList_currentRowChanged(int row)
{
	if (refreshing_) return;

	updateControls();
}

void SpeciesTab::on_IsotopologueList_itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	// Get Isotopologue pointer
	Isotopologue* isotopologue = VariantPointer<Isotopologue>(item->data(Qt::UserRole));
	if (!isotopologue) return;

	// Need to ensure new name is unique within the Species
	isotopologue->setName(species_->uniqueIsotopologueName(qPrintable(item->text()), isotopologue));

	// Make sure the item text is consistent with the Isotopologue name
	item->setText(isotopologue->name());

	dissolveWindow_->setModified();
}

void SpeciesTab::on_IsotopeTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get current Isotopologue
	Isotopologue* iso = currentIsotopologue();
	if (!iso) return;

	// Get row from the passed widget, and get target AtomType from column 0
	QTableWidgetItem* item = ui_.IsotopeTable->item(w->row(), 0);
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
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Isotopes table.\n", w->column());
			break;
	}
}

void SpeciesTab::on_AtomAddButton_clicked(bool checked)
{
	SpeciesAtom* atom = species_->addAtom(&Elements::element(1), Vec3<double>());

	refreshing_ = true;

	TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this, &SpeciesTab::updateAtomTableRow);

	refreshing_ = false;

	dissolveWindow_->setModifiedAndInvalidated();
}

void SpeciesTab::on_AtomRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
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
		// AtomType
		case (1):
			// Check the text to see if it is an existing AtomType - if not, we should create it
			atomType = dissolve_.findAtomType(qPrintable(w->text()));
			if (!atomType)
			{
				atomType = dissolve_.addAtomType(speciesAtom->element());
				atomType->setName(qPrintable(w->text()));
			}
			speciesAtom->setAtomType(atomType);
			dissolveWindow_->setModifiedAndInvalidated();
			break;
		// Coordinates
		case (2):
		case (3):
		case (4):
			speciesAtom->setCoordinate(w->column()-1, w->text().toDouble());
			dissolveWindow_->setModifiedAndInvalidated();
			break;
		// Charge
		case (5):
			speciesAtom->setCharge(w->text().toDouble());
			// TODO This change needs to be propagated to all Configurations->Molecules based on this Species
			dissolveWindow_->setModified();
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of SpeciesAtom table.\n", w->column());
			break;
	}
}

void SpeciesTab::on_BondAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_BondRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_BondTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_ || (!species_)) return;

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
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			if (species_->reconnectBond(speciesBond, i, j))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (2):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterBond(qPrintable(w->text()));
				speciesBond->setMasterParameters(master);
			}
			else
			{
				SpeciesBond::BondFunction bf = SpeciesBond::bondFunction(qPrintable(w->text()));
				speciesBond->setMasterParameters(NULL);
				speciesBond->setForm(bf);
			}
			updateRow = true;
			dissolveWindow_->setModified();
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

void SpeciesTab::on_AngleAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_AngleRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_AngleTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_ || (!species_)) return;

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
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui_.BondTable->item(w->row(), 2)->text().toInt() - 1;
			if (species_->reconnectAngle(speciesAngle, i, j, k))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (3):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterAngle(qPrintable(w->text()));
				speciesAngle->setMasterParameters(master);
			}
			else
			{
				SpeciesAngle::AngleFunction af = SpeciesAngle::angleFunction(qPrintable(w->text()));
				speciesAngle->setMasterParameters(NULL);
				speciesAngle->setForm(af);
			}
			updateRow = true;
			dissolveWindow_->setModified();
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

void SpeciesTab::on_TorsionAddButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_TorsionRemoveButton_clicked(bool checked)
{
	printf("NOT IMPLEMENTED YET!\n");
}

void SpeciesTab::on_TorsionTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_ || (!species_)) return;

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
			i = ui_.BondTable->item(w->row(), 0)->text().toInt() - 1;
			j = ui_.BondTable->item(w->row(), 1)->text().toInt() - 1;
			k = ui_.BondTable->item(w->row(), 2)->text().toInt() - 1;
			l = ui_.BondTable->item(w->row(), 3)->text().toInt() - 1;
			if (species_->reconnectTorsion(speciesTorsion, i, j, k, l))
			{
				updateRow = true;
				dissolveWindow_->setModified();
			}
			break;
		// Functional Form
		case (4):
			// If the text starts with an '@' then its a reference to a master term
			if (w->text().at(0) == '@')
			{
				MasterIntra* master = dissolve_.coreData().hasMasterTorsion(qPrintable(w->text()));
				speciesTorsion->setMasterParameters(master);
			}
			else
			{
				SpeciesTorsion::TorsionFunction tf = SpeciesTorsion::torsionFunction(qPrintable(w->text()));
				speciesTorsion->setMasterParameters(NULL);
				speciesTorsion->setForm(tf);
			}
			updateRow = true;
			dissolveWindow_->setModified();
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

// Read widget state through specified LineParser
bool SpeciesTab::readState(LineParser& parser, const CoreData& coreData)
{
	return true;
}

// Write widget state through specified LineParser
bool SpeciesTab::writeState(LineParser& parser)
{
	return true;
}
