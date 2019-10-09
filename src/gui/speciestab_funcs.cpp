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
#include "gui/delegates/null.h"
#include "gui/getspeciesnamedialog.h"
#include "gui/selectforcefielddialog.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/helpers/treewidgetupdater.h"
#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include "classes/speciesbond.h"
#include "data/fflibrary.h"
#include <QListWidgetItem>
#include <QMessageBox>

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
	// -- Isotopologues Tree
	ui_.IsotopologuesTree->setItemDelegateForColumn(1, new NullDelegate(this));
	ui_.IsotopologuesTree->setItemDelegateForColumn(2, new IsotopeComboDelegate(this));

	// Ensure fonts for table headers are set correctly and the headers themselves are visible
	ui_.AtomTable->horizontalHeader()->setFont(font());
	ui_.AtomTable->horizontalHeader()->setVisible(true);
	ui_.BondTable->horizontalHeader()->setFont(font());
	ui_.BondTable->horizontalHeader()->setVisible(true);
	ui_.AngleTable->horizontalHeader()->setFont(font());
	ui_.AngleTable->horizontalHeader()->setVisible(true);
	ui_.TorsionTable->horizontalHeader()->setFont(font());
	ui_.TorsionTable->horizontalHeader()->setVisible(true);

	// Set up SpeciesViewer
	ui_.ViewerWidget->setCoreData(&dissolve.coreData());
	ui_.ViewerWidget->speciesViewer()->setSpecies(species_);

	// Connect signals / slots
	connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(dataChanged()), this, SLOT(updateControls()));
	connect(ui_.ViewerWidget->speciesViewer(), SIGNAL(dataModified()), dissolveWindow_, SLOT(setModified()));

	refreshing_ = false;
}

SpeciesTab::~SpeciesTab()
{
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType SpeciesTab::type() const
{
	return MainTab::SpeciesTabType;
}

// Raise suitable dialog for entering / checking new tab name
QString SpeciesTab::getNewTitle(bool& ok)
{
	// Get a new, valid name for the Species
	GetSpeciesNameDialog nameDialog(this, dissolve_.coreData());
	ok = nameDialog.get(species_, species_->name());

	if (ok)
	{
		// Rename our Species, and flag that our data has been modified
		species_->setName(qPrintable(nameDialog.newName()));

		dissolveWindow_->setModified();
	}

	return nameDialog.newName();
}

// Return whether the title of the tab can be changed
bool SpeciesTab::canChangeTitle() const
{
	return true;
}

/*
 * Species Target
 */

// Return displayed Species
Species* SpeciesTab::species() const
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

// IsotopologuesTree top-level update function
void SpeciesTab::updateIsotopologuesTreeTopLevelItem(QTreeWidget* treeWidget, int topLevelItemIndex, Isotopologue* data, bool createItem)
{
	QTreeWidgetItem* item;
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(0, Qt::UserRole, VariantPointer<Isotopologue>(data));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		treeWidget->insertTopLevelItem(topLevelItemIndex, item);
	}
	else item = treeWidget->topLevelItem(topLevelItemIndex);

	// Set item data
	item->setText(0, data->name());

	// Update child items
	TreeWidgetRefDataListUpdater<SpeciesTab,AtomType,Isotope*> isotopeUpdater(item, data->isotopes(), this, &SpeciesTab::updateIsotopologuesTreeChildItem);
}

// IsotopologuesTree item update function
void SpeciesTab::updateIsotopologuesTreeChildItem(QTreeWidgetItem* parentItem, int childIndex, AtomType* atomType, Isotope* isotope, bool createItem)
{
	QTreeWidgetItem* item;

	// AtomType
	if (createItem)
	{
		item = new QTreeWidgetItem;
		item->setData(1, Qt::UserRole, VariantPointer<AtomType>(atomType));
		item->setData(2, Qt::UserRole, VariantPointer<Isotope>(isotope));
		item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
		parentItem->insertChild(childIndex, item);
	}
	else item = parentItem->child(childIndex);
	item->setText(1, atomType->name());
	item->setText(2, IsotopeComboDelegate::textForIsotope(isotope));
}

// Update controls in tab
void SpeciesTab::updateControls()
{
	refreshing_ = true;

	// View / Generate Tab
	ui_.ForcefieldButton->setText(species_ && species_->forcefield() ? species_->forcefield()->name() : "<None>");
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

	// Isotopologues Tab
	// -- Isotopologues Tree
	if (!species_) ui_.IsotopologuesTree->clear();
	else
	{
		TreeWidgetUpdater<SpeciesTab,Isotopologue> isotopologueUpdater(ui_.IsotopologuesTree, species_->isotopologues(), this, &SpeciesTab::updateIsotopologuesTreeTopLevelItem);

		// If there is no current isotopologue selected, try to select the first
		if (!currentIsotopologue()) ui_.IsotopologuesTree->setCurrentItem(ui_.IsotopologuesTree->topLevelItem(0));
	}
	Isotopologue* isotopologue = currentIsotopologue();
	ui_.IsotopologueRemoveButton->setEnabled(isotopologue != NULL);

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
	// Get current item from tree, and check the parent item
	QTreeWidgetItem* item = ui_.IsotopologuesTree->currentItem();
	if (!item) return NULL;
	if (item->parent()) return VariantPointer<Isotopologue>(item->parent()->data(0, Qt::UserRole));
	else return VariantPointer<Isotopologue>(item->data(0, Qt::UserRole));
}

// View / Generate
void SpeciesTab::on_ForcefieldButton_clicked(bool checked)
{
	// Select the desired Forcefield
	static SelectForcefieldDialog selectForcefieldDialog(this, ForcefieldLibrary::forcefields());
	Forcefield* ff = selectForcefieldDialog.selectForcefield(species_->forcefield());
	if (!ff) return;

	// Confirm, and ask whether to overwrite existing forcefield terms
	QMessageBox queryBox;
	queryBox.setText("Would you like to clear current terms from the Species and reapply the selected forcefield?");
	queryBox.setInformativeText("Reapply Terms?");
	queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
	queryBox.setDefaultButton(QMessageBox::Cancel);
	int ret = queryBox.exec();

	if (ret == QMessageBox::Cancel) return;

	// Set the new Forcefield on the Species
	species_->setForcefield(ff);

	if (ret == QMessageBox::Yes) species_->applyForcefieldTerms(dissolve_.coreData());

	updateControls();
}

void SpeciesTab::on_ForcefieldAutoApplyCheck_clicked(bool checked)
{
	// TODO
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

void SpeciesTab::on_IsotopologueGenerateButton_clicked(bool checked)
{
	// TODO
}

void SpeciesTab::on_IsotopologueExpandAllButton_clicked(bool checked)
{
	ui_.IsotopologuesTree->expandAll();
}

void SpeciesTab::on_IsotopologueCollapseAllButton_clicked(bool checked)
{
	ui_.IsotopologuesTree->collapseAll();
}

void SpeciesTab::on_IsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (refreshing_ || (!item)) return;

	Isotopologue* isotopologue = currentIsotopologue();

	// If a top-level item, then the only possibility is to edit the isotopologue name (column 0)
	if (item->parent() == NULL)
	{
		// Name of the isotopologue
		if (column == 0)
		{
			// Name of the Isotopologue has been changed - make sure it doesn't exist in the Species already
			ListIterator<Isotopologue> topeIterator(species_->isotopologues());
			while (Isotopologue* tope = topeIterator.iterate())
			{
				if (isotopologue == tope) continue;

				if (item->text(0) == tope->name())
				{
					isotopologue->setName(species_->uniqueIsotopologueName(qPrintable(item->text(0))));
					Messenger::warn("An Isotopologue named '%s' already exists in the Species, so it has been renamed to '%s'.\n", qPrintable(item->text(0)), isotopologue->name());
					break;
				}
			}
		}
	}
	else if (column == 1)
	{
		// AtomType - no action to perform (not editable)
	}
	else if (column == 2)
	{
		// Set neutron isotope - need to get AtomType from column 1...
		AtomType* atomType = VariantPointer<AtomType>(item->data(1, Qt::UserRole));
		Isotope* isotope = VariantPointer<Isotope>(item->data(2, Qt::UserRole));
		if (isotope) isotopologue->setAtomTypeIsotope(atomType, isotope);
		dissolveWindow_->setModified();
	}
}

void SpeciesTab::on_AtomAddButton_clicked(bool checked)
{
	SpeciesAtom* atom = species_->addAtom(&Elements::element(1), Vec3<double>());

	refreshing_ = true;

	TableWidgetUpdater<SpeciesTab,SpeciesAtom> speciesAtomUpdater(ui_.AtomTable, species_->atoms(), this, &SpeciesTab::updateAtomTableRow);

	refreshing_ = false;

	dissolveWindow_->setModified();
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
			dissolveWindow_->setModified();
			break;
		// Coordinates
		case (2):
		case (3):
		case (4):
			speciesAtom->setCoordinate(w->column()-1, w->text().toDouble());
			dissolveWindow_->setModified();
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
