/*
	*** Main Window - SpeciesTab Functions
	*** src/gui/mainwindow_species.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/mainwindow.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "base/ptable.h"
#include "base/sysfunc.h"

// Refresh tab
void MainWindow::refreshSpeciesTab(bool listGroup, bool atomTypesGroup, bool viewGroup, bool grainsGroup, bool isotopologuesGroup)
{
	// Set the refreshing_ flag
	speciesTabRefreshing_ = true;
	
	// Update the title while we're here, just in case
	updateTitle();

	// Refresh the Species list?
	if (listGroup)
	{
		// Grab current selected item
		int oldRow = ui.DefinedSpeciesList->currentRow();

		// Re-populate list
		ui.DefinedSpeciesList->clear();
		QListWidgetItem* item;
		for (Species* sp = dUQ_.species(); sp != NULL; sp = sp->next)
		{
			item = new QListWidgetItem(ui.DefinedSpeciesList);
			item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setText(sp->name());
		}

		// Re-select the same index as before (if possible)
		if ((oldRow != -1) && (oldRow < ui.DefinedSpeciesList->count())) ui.DefinedSpeciesList->setCurrentRow(oldRow);
		else ui.DefinedSpeciesList->setCurrentRow(0);
		
		// Change group title
		Dnchar title(-1, "Defined Species (%i)", ui.DefinedSpeciesList->count());
		ui.DefinedSpeciesGroup->setTitle(title.get());
	}

	// Grab current Species for remaining panels
	if ((ui.DefinedSpeciesList->currentRow() == -1) && (dUQ_.nSpecies() > 0)) ui.DefinedSpeciesList->setCurrentRow(dUQ_.nSpecies()-1);
	Species* currentSpecies = selectedSpecies();
	ui.ViewEditGroup->setEnabled(currentSpecies);
	ui.IntramolecularTermsGroup->setEnabled(currentSpecies);
	ui.GrainsGroup->setEnabled(currentSpecies);
	ui.IsotopologuesGroup->setEnabled(currentSpecies);
	ui.RemoveSpeciesButton->setEnabled(currentSpecies);

	// Refresh the Atom Types group?
	if (atomTypesGroup)
	{
		// Re-populate list
		ui.DefinedAtomTypesTree->clear();

		int count = 0;
		QTreeWidgetItem* item;
		QComboBox* combo;
		Parameters* params;

		for (AtomType* at = dUQ_.atomTypes(); at != NULL; at = at->next)
		{
		
			item = new QTreeWidgetItem(ui.DefinedAtomTypesTree);

			item->setData(0, Qt::UserRole, QVariant(count));
			item->setText(0, at->name());
			item->setText(1, PeriodicTable::element(at->element()).symbol());
			
			// Create a combo box with the available Parameters for this AtomType
			combo = new QComboBox(this);
			combo->setMinimumSize(100,20);
			for (params = PeriodicTable::element(at->element()).parameters(); params != NULL; params = params->next)
			{
				combo->addItem(params->description());
				if (params == at->parameters()) combo->setCurrentIndex(combo->count()-1);
			}
			combo->setItemData(0, QVariant(count));
			ui.DefinedAtomTypesTree->setItemWidget(item, 2, combo);
			QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(atomTypeParametersChanged(int)));

			item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			++count;
		}

		for (int n = 0; n < 3; ++n) ui.DefinedAtomTypesTree->resizeColumnToContents(n);
		
		// Change group title
		Dnchar title(-1, "AtomType Definitions (%i)", dUQ_.nAtomTypes());
		ui.DefinedAtomTypesGroup->setTitle(title.get());

		// Set control button states
		AtomType* at = selectedAtomType();
		ui.CopyAtomTypeButton->setEnabled(at);
		ui.RemoveAtomTypeButton->setEnabled(at);
	}

	// Refresh the Species Grains list?
	if (grainsGroup)
	{
		// Grab current selected item
		int oldRow = ui.GrainsTable->currentRow();

		// Re-populate list
		ui.GrainsTable->clearContents();
		
		if (currentSpecies)
		{
			Dnchar text(1024);
			int count = 0;
			QTableWidgetItem* item;
			
			ui.GrainsTable->setRowCount(currentSpecies->nGrainDefinitions());
			
			for (GrainDefinition* gd = currentSpecies->grainDefinitions(); gd != NULL; gd = gd->next)
			{
				item = new QTableWidgetItem(gd->name());
				item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				ui.GrainsTable->setItem(count, 0, item);

				item = new QTableWidgetItem(itoa(gd->nAtoms()));
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				ui.GrainsTable->setItem(count, 1, item);

				text.clear();
				for (RefListItem<Atom,int>* ri = gd->atoms(); ri != NULL; ri = ri->next) text.strcatf("%i ", ri->item->userIndex());
				item = new QTableWidgetItem(text.get());
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				ui.GrainsTable->setItem(count, 2, item);

				++count;
			}
			ui.GrainsTable->resizeColumnsToContents();

			// Re-select the same index as before (if possible)
			if ((oldRow != -1) && (oldRow < ui.GrainsTable->rowCount())) ui.GrainsTable->setCurrentCell(oldRow,0);
			else ui.GrainsTable->setCurrentCell(0,0);
		}

		// Change group title
		Dnchar title(-1, "Grain Definitions (%i)", ui.GrainsTable->rowCount());
		ui.GrainsGroup->setTitle(title.get());
	}

	// Refresh the Species isotopologues list?
	if (isotopologuesGroup)
	{
		// Re-populate list
		ui.IsotopologuesTree->clear();

		if (currentSpecies)
		{
			QTreeWidgetItem* topItem, *item;
			RefList<Isotopologue,int> isoList;
			QComboBox* combo;
			QStringList data;
			Dnchar text;
			
			// Every item added to the list will *at least* have a QVariant with the Isotopologue name contained within it
			
			// Loop over defined Isotopologues - these will constitute top-level items
			for (Isotopologue* iso = currentSpecies->isotopologues(); iso != NULL; iso = iso->next)
			{
				topItem = new QTreeWidgetItem(ui.IsotopologuesTree);
				topItem->setText(0, iso->name());
				topItem->setExpanded(true);
				topItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable	);
				data.clear();
				data << iso->name();
				topItem->setData(0, Qt::UserRole, QVariant(data));

				// Check for no Isotopologues in list...
				if (!iso->isotopes())
				{
					item = new QTreeWidgetItem(topItem);
					item->setText(0, "<< No AtomTypes Defined >>");
					continue;
				}

				// Now cycle over AtomTypes present in this Isotopologue (Species)
				for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next)
				{
					item = new QTreeWidgetItem(topItem);
					// QVariant data will contain the Isotopologue and AtomType names (Isotope can be determined from ComboBox index)
					data.clear();
					data << iso->name();
					data << ri->item->name();
					item->setData(0, Qt::UserRole, QVariant(data));

					// Create a combo box with all possible Isotopes, selecting the current one
					combo = new QComboBox(this);
					combo->setMinimumSize(100,20);
					for (Isotope* tope = PeriodicTable::element(ri->item->element()).isotopes(); tope != NULL; tope = tope->next)
					{
						if (tope->A() == 0) text.sprintf("Natural %f (bc = %f)", tope->atomicWeight(), tope->boundCoherent());
						else text.sprintf("A=%i %f (bc = %f)", tope->A(), tope->atomicWeight(), tope->boundCoherent());
						combo->addItem(text.get());
						if (tope == ri->data) combo->setCurrentIndex(combo->count()-1);
					}
					combo->setItemData(0, QVariant(data), Qt::UserRole);
					QObject::connect(combo, SIGNAL(activated(int)), this, SLOT(isotopologueIsotopeChanged(int)));
					ui.IsotopologuesTree->setItemWidget(item, 3, combo);

					item->setText(1, ri->item->name());
					item->setText(2, PeriodicTable::element(ri->item->element()).symbol());
					item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				}
			}

			// Resize columns to fit contents
			for (int n=0; n<5; ++n) ui.IsotopologuesTree->resizeColumnToContents(n);
		}

		// Change group title
		Dnchar title(-1, "Isotopologues (%i)", currentSpecies ? currentSpecies->nIsotopologues() : 0);
		ui.IsotopologuesGroup->setTitle(title.get());
	}

	// Refresh the Species view?
	if (viewGroup)
	{
		// Make sure the currently-selected Species is the one displayed in the panel
		ui.SpeciesViewer->setSource(currentSpecies);
		
		if (currentSpecies)
		{
			// Set visible GrainDefinition (if any)
			GrainDefinition* currentGrain = ui.GrainsTable->currentRow() == -1 ? NULL : currentSpecies->grainDefinition(ui.GrainsTable->currentRow());
			currentSpecies->setHighlightedGrainDefinition(currentGrain);

			// Set visible Isotopologue (if any)
			Isotopologue* iso = selectedIsotopologue();
			currentSpecies->setHighlightedIsotopologue(iso);
		}

		// Based on current selection, (de)activate controls
		int nSelected = (currentSpecies ? currentSpecies->nSelectedAtoms() : 0);
		ui.AddBondFromSelectionButton->setEnabled(nSelected == 2);
		ui.AddAngleFromSelectionButton->setEnabled(nSelected == 3);
		ui.AddGrainFromSelectionButton->setEnabled(nSelected > 0);

		ui.SpeciesViewer->postRedisplay();
	}

	// Unset the refreshing_ flag
	speciesTabRefreshing_ = false;
}

/*
 * General Functions
 */

// Return current selected Species, if any
Species* MainWindow::selectedSpecies()
{
	// Get current row in SpeciesList
	int currentRow = ui.DefinedSpeciesList->currentRow();
	if (currentRow == -1) return NULL;
	return dUQ_.species(currentRow);
}

// Return current selected GrainDefinition, if any
GrainDefinition* MainWindow::selectedGrainDefinition()
{
	// First, need a valid Species
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return NULL;
	
	// Next, need row selected in GrainsTable
	int grainRow = ui.GrainsTable->currentRow();
	if (grainRow == -1) return NULL;
	
	// Finally, retrieve nth GrainDefinition in Species
	GrainDefinition* gd = currentSpecies->grainDefinition(grainRow);
	return gd;
}

// Return current selected Isotopologue, if any
Isotopologue* MainWindow::selectedIsotopologue()
{
	// First, need a valid Species
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return NULL;

	QTreeWidgetItem* item = ui.IsotopologuesTree->currentItem();
	if (item == NULL) return NULL;

	QVariant data = item->data(0, Qt::UserRole);
	if (!data.isValid()) return NULL;
	
	Isotopologue* iso = currentSpecies->findIsotopologue(qPrintable(data.toStringList().at(0)));
	return iso;
}

// Return current selected AtomType, if any
AtomType* MainWindow::selectedAtomType()
{
	QTreeWidgetItem* item = ui.DefinedAtomTypesTree->currentItem();
	if (item == NULL) return NULL;

	QVariant data = item->data(0, Qt::UserRole);
	if (!data.isValid()) return NULL;
	
	AtomType* at = dUQ_.atomType(data.toInt());
	return at;
}

/*
 * Species Group
 */

// Different Species selected in list
void MainWindow::on_DefinedSpeciesList_currentRowChanged(int row)
{
	if (speciesTabRefreshing_) return;
	refresh(ViewEditGroup | GrainsGroup | IsotopologuesGroup);
}

// Item changed (i.e. text changed)
void MainWindow::on_DefinedSpeciesList_itemChanged(QListWidgetItem* item)
{
	if (speciesTabRefreshing_) return;

	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;

	currentSpecies->setName(dUQ_.uniqueSpeciesName(qPrintable(item->text())));

	// Update dependent data
	refreshExperimentTab();
}

// Add a new Species to the Model
void MainWindow::on_AddSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = dUQ_.addSpecies();
	currentSpecies->setName(dUQ_.uniqueSpeciesName("Unnamed Species", currentSpecies));

	// Refresh SpeciesTab and ExperimentTab
	refreshSpeciesTab();
	refreshExperimentTab();
}

// Remove the current Species from the Model
void MainWindow::on_RemoveSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	ui.SpeciesViewer->unsetSource();

	dUQ_.removeSpecies(currentSpecies);

	// Refresh SpeciesTab and ExperimentTab
	refreshSpeciesTab();
	refreshExperimentTab();
}

// Load Species data from a file
void MainWindow::on_LoadSpeciesButton_clicked(bool checked)
{
	// Create a file selector
	static QDir currentDir(getenv("PWD"));
	QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open Model File", currentDir.path(), "XYZ Files (*.xyz)");
	
	// Was at least one file selected?
	if (fileNames.count() == 0) return;
	
	for (int n=0; n<fileNames.count(); ++n) dUQ_.loadSpecies(qPrintable(fileNames[n]));

	// Refresh SpeciesTab and ExperimentTab
	refreshSpeciesTab();
	refreshExperimentTab();

	// Select last loaded Species
	ui.DefinedSpeciesList->setCurrentRow(ui.DefinedSpeciesList->count()-1);
}

// Save Species definition to a file
void MainWindow::on_SaveSpeciesButton_clicked(bool checked)
{
	// TODO
}

/*
 * Species Viewer
 */

// Add Bond from current Atom selection
void MainWindow::on_AddBondFromSelectionButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (!currentSpecies) return;

	// Check that the correct number of Atoms is selected
	if (currentSpecies->nSelectedAtoms() != 2)
	{
		printf("GUI_ERROR - Exactly two Atoms must be selected in order to add a Bond.\n");
		return;
	}

	// Get selected atoms
	Atom* atoms[2];
	for (int n=0; n<2; ++n) atoms[n] = currentSpecies->selectedAtom(n);
	currentSpecies->addBond(atoms[0], atoms[1]);
	
	// Clear selection and refresh display
	currentSpecies->clearAtomSelection();
	refresh(ViewEditGroup | IntramolecularTermsGroup);
}

// Add Angle from current Atom selection
void MainWindow::on_AddAngleFromSelectionButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (!currentSpecies) return;

	// Check that the correct number of Atoms is selected
	if (currentSpecies->nSelectedAtoms() != 3)
	{
		printf("GUI_ERROR - Exactly three Atoms must be selected in order to add an Angle.\n");
		return;
	}

	// Get selected atoms
	Atom* atoms[3];
	for (int n=0; n<3; ++n) atoms[n] = currentSpecies->selectedAtom(n);
	currentSpecies->addAngle(atoms[0], atoms[1], atoms[2]);
	
	// Clear selection and refresh display
	currentSpecies->clearAtomSelection();
	refresh(ViewEditGroup | IntramolecularTermsGroup);
}

// Add Grain from current Atom selection
void MainWindow::on_AddGrainFromSelectionButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (!currentSpecies) return;

	// Check that the correct number of Atoms is selected
	if (currentSpecies->nSelectedAtoms() == 0)
	{
		printf("GUI_ERROR - At least one Atom must be selected in order to add a GrainDefinition.\n");
		return;
	}

	// Create new GrainDefinition and add selected Atoms to it
	GrainDefinition* gd = currentSpecies->addGrainDefinition();
	for (RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms(); ri != NULL; ri = ri->next) currentSpecies->addAtomToGrainDefinition(ri->item, gd);
	gd->setName(currentSpecies->uniqueGrainDefinitionName(gd->nameFromAtoms()));

	// Update the list of GrainDefinitions (removing empty ones etc.)
	currentSpecies->updateGrainDefinitions();
	
	// Clear selection and refresh display
	currentSpecies->clearAtomSelection();
	refresh(ViewEditGroup | GrainsGroup);
}

// View AtomType Check clicked
void MainWindow::on_ViewAtomTypeCheck_clicked(bool checked)
{
	refresh(ViewEditGroup);
}

// View Index Check clicked
void MainWindow::on_ViewIndexCheck_clicked(bool checked)
{
	refresh(ViewEditGroup);
}

/*
 * Atom Types Group
 */

// Combo slot
void MainWindow::atomTypeParametersChanged(int index)
{
	if (speciesTabRefreshing_) return;

	// Data associated to column 0 gives us the AtomType index
	QComboBox *combo = static_cast<QComboBox*>(sender());
	if (!combo) return;

	QVariant data = combo->itemData(0, Qt::UserRole);
	if (!data.isValid()) return;
	
	
	AtomType* at = dUQ_.atomType(data.toInt());
	if (at == NULL) return;
	
	at->setParameters(PeriodicTable::element(at->element()).parameters(index));

	// Update
	dUQ_.updateAtomTypes();
	refresh(ViewEditGroup | DefinedAtomTypesGroup);
}

// Item clicked in AtomTypesTree
void MainWindow::on_DefinedAtomTypesTree_itemClicked(QTreeWidgetItem* item, int column)
{
	// Get selected AtomType
	AtomType* at = selectedAtomType();
	ui.CopyAtomTypeButton->setEnabled(at);
	ui.RemoveAtomTypeButton->setEnabled(at);
}

// Item edited in AtomTypesTree
void MainWindow::on_DefinedAtomTypesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (speciesTabRefreshing_) return;

	// Get selected AtomType
	AtomType* at = selectedAtomType();
	if (at == NULL) return;

	// Which column was edited?
	if (column == 0) at->setName(dUQ_.uniqueAtomTypeName(qPrintable(item->text(column)), at));
	else if (column == 1)
	{
		// Element of AtomType changed...
		int el = PeriodicTable::find(qPrintable(item->text(1)));
		if (el == -1) msg.print("Ignored AtomType edit - '%s' is not a valid chemical element.\n", qPrintable(item->text(1)));
		else
		{
			// Safest option here is to copy the existing AtomType data, remove the existing AtomType,
			// and then add a new one, updating the AtomTypes list afterwards.
			AtomType oldType;
			oldType.setName(at->name());
			oldType.setParameters(at->parameters());
			dUQ_.removeAtomType(at);
			at = dUQ_.addAtomType(el);
			at->setName(oldType.name());
			at->setParameters(oldType.parameters());
			
			dUQ_.updateAtomTypes();
			dUQ_.updateIsotopologues();
		}
	}

	// Update
	refresh(ViewEditGroup | IsotopologuesGroup | DefinedAtomTypesGroup);
}

// Add AtomType Button
void MainWindow::on_AddAtomTypeButton_clicked(bool checked)
{
	AtomType* newType = dUQ_.addAtomType(1);

	// Update
	refresh(DefinedAtomTypesGroup);
}

// Copy AtomType definition
void MainWindow::on_CopyAtomTypeButton_clicked(bool checked)
{
	// Get selected AtomType
	AtomType* at = selectedAtomType();
	if (at == NULL) return;

	// Add a copy of this AtomType, changing the name...
	AtomType* newType = dUQ_.addAtomType(at->element());
	newType->setParameters(at->parameters());

	// Update
	refresh(DefinedAtomTypesGroup);
}

// Remove AtomType definition
void MainWindow::on_RemoveAtomTypeButton_clicked(bool checked)
{
	// Get selected AtomType
	AtomType* at = selectedAtomType();
	if (at == NULL) return;

	dUQ_.removeAtomType(at);

	// Update
	refresh(ViewEditGroup | IsotopologuesGroup | DefinedAtomTypesGroup);
}

/*
 * Grains Group
 */

// Different grain selected in table
void MainWindow::on_GrainsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if (speciesTabRefreshing_) return;
	refresh(ViewEditGroup);
}

// Grain information changed
void MainWindow::on_GrainsTable_cellChanged(int row, int column)
{
	if (speciesTabRefreshing_) return;
	// TODO
}

// Add new Grain to Species
void MainWindow::on_AddGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	GrainDefinition* gd = currentSpecies->addGrainDefinition();
	gd->setName(currentSpecies->uniqueGrainDefinitionName("NewGrain"));

	refresh(GrainsGroup);
}

// Remove the current Grain from the Model
void MainWindow::on_RemoveGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	GrainDefinition* gd = selectedGrainDefinition();
	
	currentSpecies->removeGrainDefinition(gd);
	
	// Update
	refresh(ViewEditGroup | GrainsGroup);
}

// Add default grain definition
void MainWindow::on_AddDefaultGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	if (currentSpecies->nGrainDefinitions() != 0)
	{
		if (QMessageBox::question(this, "Add Default Grain", "All existing GrainDefinitions will be lost - proceed?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok) return;
	}
	
	currentSpecies->addDefaultGrainDefinition();

	// Update
	refresh(ViewEditGroup | GrainsGroup);
}

// Auto-add grains
void MainWindow::on_AutoGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	if (currentSpecies->nGrainDefinitions() != 0)
	{
		if (QMessageBox::question(this, "Auto Add Grains", "All existing GrainDefinitions will be lost - proceed?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok) return;
	}
	
	currentSpecies->autoAddGrains();
	
	// Update
	refresh(ViewEditGroup | GrainsGroup);
}

/*
 * Isotopologues Group
 */

// Determine Isotopologue and AtomType from QVariant data
bool MainWindow::getIsotopologueAndAtomType(QStringList data, Isotopologue*& iso, AtomType*& at)
{
	if (data.count() != 2)
	{
		printf("GUI_ERROR - QStringList must contain exactly two items (Isotopologue and AtomType names).\n");
		return false;
	}
	
	// Get current species
	Species* currentSpecies = selectedSpecies();
	if (!currentSpecies) return false;
	
	// Now we have the species, determine the isotopologue...
	iso = currentSpecies->findIsotopologue(qPrintable(data[0]));
	if (!iso)
	{
		printf("GUI_ERROR - QVariant data specifies an unknown Isotopologue ('%s').\n", qPrintable(data[0]));
		return false;
	}
	
	// Now find AtomType
	at = dUQ_.findAtomType(qPrintable(data[1]));
	if (!at)
	{
		printf("GUI_ERROR - QVariant data specifies an unknown AtomType ('%s').\n", qPrintable(data[1]));
		return false;
	}

	return true;
}

// Different Isotopologue selected in list
void MainWindow::on_IsotopologuesTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (speciesTabRefreshing_) return;
	refresh(ViewEditGroup);
}

// Item changed (i.e. text changed)
void MainWindow::on_IsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (speciesTabRefreshing_) return;

	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = selectedIsotopologue();
	if (iso == NULL) return;
	
	// Check current text - if its the same, do nothing. If different, then set a (unique) version of it
	if (item->text(0) != iso->name()) iso->setName(currentSpecies->uniqueIsotopologueName(qPrintable(item->text(0)),iso));
	
	// Update
	refresh(IsotopologuesGroup | AssignedIsotopologuesGroup);
}

// Add Isotopologue to Species
void MainWindow::on_AddIsotopologueToSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = currentSpecies->addIsotopologue("New Isotopologue");
	dUQ_.updateIsotopologues(currentSpecies, iso);

	// Update
	refresh(IsotopologuesGroup | AssignedIsotopologuesGroup);
}

// Remove Isotopologue from Species
void MainWindow::on_RemoveIsotopologueFromSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = selectedIsotopologue();
	if (iso == NULL) return;
	
	dUQ_.removeSpeciesIsotopologue(currentSpecies, iso);
	
	// Update
	refresh(IsotopologuesGroup | AssignedIsotopologuesGroup);
}

// Copy Isotopologue
void MainWindow::on_CopyIsotopologueButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = selectedIsotopologue();
	if (iso == NULL) return;
	
	Isotopologue* newTope = currentSpecies->addIsotopologue(currentSpecies->uniqueIsotopologueName(iso->name()));
	dUQ_.updateIsotopologues(currentSpecies, newTope);
	for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next) newTope->setAtomTypeIsotope(ri->item, ri->data);
	
	// Update
	refresh(ViewEditGroup | IsotopologuesGroup | AssignedIsotopologuesGroup);
}

// Different Isotope selected for AtomType in Isotopologue
void MainWindow::isotopologueIsotopeChanged(int index)
{
	if (experimentTabRefreshing_) return;

	// Need to get the QComboBox which sent the signal
	QComboBox *combo = static_cast<QComboBox*>(sender());
	if (!combo) return;
	
	// The QVariant data attached to the combo item contains Species and Isotopologue names
	QVariant variant = combo->itemData(0);
	if (!variant.isValid())
	{
		printf("GUI_ERROR - Failed to get QVariant data from QComboBox sender.\n");
		return;
	}
	QStringList data = variant.toStringList();
	Isotopologue* iso;
	AtomType* at;
	if (!getIsotopologueAndAtomType(data, iso, at)) return;
	Species* currentSpecies = selectedSpecies();
	if (!currentSpecies) return;
	
	// Get new Isotope pointer (from current combo index)
	Isotope* newTope = PeriodicTable::element(at->element()).isotope(combo->currentIndex());
	if (!newTope)
	{
		printf("GUI_ERROR - QComboBox index does not specify an Isotope for element %s.\n", PeriodicTable::element(at->element()).symbol());
		return;
	}

	// Set entry in Isotopologue
	iso->setAtomTypeIsotope(at, newTope);
	
	// Update
	refresh(ViewEditGroup | AssignedIsotopologuesGroup);
}
