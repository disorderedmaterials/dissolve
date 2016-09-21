/*
	*** Species Dialog
	*** src/gui/species_funcs.cpp
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

#include "gui/species.h"
#include "classes/atomtype.h"
#include "classes/species.h"

// Constructor
DUQSpeciesDockWidget::DUQSpeciesDockWidget(QWidget* parent, DUQObject& duq) : QDockWidget(parent), dUQ_(duq)
{
	// Call the main creation function
	ui.setupUi(this);

	// Set initial variables
	refreshing_ = false;
}

// Destructor
DUQSpeciesDockWidget::~DUQSpeciesDockWidget()
{
}

// Window close event
void DUQSpeciesDockWidget::closeEvent(QCloseEvent* event)
{
}

// General refresh function
void DUQSpeciesDockWidget::refresh(int targets)
{
	refreshing_ = true;

	// Refresh the Species list?
	if (DockWidgetPanel::SpeciesListPanel)
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
// 	ui.IntramolecularTermsGroup->setEnabled(currentSpecies);
	ui.GrainsTab->setEnabled(currentSpecies);
	ui.IsotopologuesTab->setEnabled(currentSpecies);
	ui.RemoveSpeciesButton->setEnabled(currentSpecies);

	// Refresh the Species Grains list?
	if (DockWidgetPanel::SpeciesGrainsPanel)
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
			
			ui.GrainsTable->setRowCount(currentSpecies->nGrains());
			
			for (SpeciesGrain* sg = currentSpecies->grains(); sg != NULL; sg = sg->next)
			{
				item = new QTableWidgetItem(sg->name());
				item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				ui.GrainsTable->setItem(count, 0, item);

				item = new QTableWidgetItem(QString::number(sg->nAtoms()));
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
				ui.GrainsTable->setItem(count, 1, item);

				text.clear();
				for (RefListItem<SpeciesAtom,int>* ri = sg->atoms(); ri != NULL; ri = ri->next) text.strcatf("%i ", ri->item->userIndex());
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

		// Change count label
		ui.NGrainsLabel->setText("NGrains = " + QString::number(ui.GrainsTable->rowCount()));
	}

	// Refresh the Species isotopologues list?
	if (DockWidgetPanel::SpeciesIsotopologuesPanel)
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

		// Change count label
		ui.NIsotopologuesLabel->setText("NIsotopologues = " + QString::number(currentSpecies ? currentSpecies->nIsotopologues() : 0));
	}

	// Refresh the Species view?
	if (DockWidgetPanel::SpeciesViewPanel)
	{
		// Make sure the currently-selected Species is the one displayed in the panel
		ui.SpeciesView->setSource(currentSpecies);
		
		if (currentSpecies)
		{
			// Set visible GrainDefinition (if any)
			SpeciesGrain* currentGrain = ui.GrainsTable->currentRow() == -1 ? NULL : currentSpecies->grain(ui.GrainsTable->currentRow());
			currentSpecies->setHighlightedGrain(currentGrain);

			// Set visible Isotopologue (if any)
			Isotopologue* iso = selectedIsotopologue();
			currentSpecies->setHighlightedIsotopologue(iso);
		}

		// Based on current selection, (de)activate controls
		int nSelected = (currentSpecies ? currentSpecies->nSelectedAtoms() : 0);
		ui.AddBondFromSelectionButton->setEnabled(nSelected == 2);
		ui.AddAngleFromSelectionButton->setEnabled(nSelected == 3);
		ui.AddGrainFromSelectionButton->setEnabled(nSelected > 0);

		ui.SpeciesView->postRedisplay();
	}

	refreshing_ = false;
}

/*
 * Private Functions
 */

// Return current selected Species, if any
Species* DUQSpeciesDockWidget::selectedSpecies()
{
	// Get current row in SpeciesList
	int currentRow = ui.DefinedSpeciesList->currentRow();
	if (currentRow == -1) return NULL;
	return dUQ_.species(currentRow);
}

// Return current selected GrainDefinition, if any
SpeciesGrain* DUQSpeciesDockWidget::selectedGrainDefinition()
{
	// First, need a valid Species
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return NULL;
	
	// Next, need row selected in GrainsTable
	int grainRow = ui.GrainsTable->currentRow();
	if (grainRow == -1) return NULL;
	
	// Finally, retrieve nth GrainDefinition in Species
	SpeciesGrain* sg = currentSpecies->grain(grainRow);
	return sg;
}

// Return current selected Isotopologue, if any
Isotopologue* DUQSpeciesDockWidget::selectedIsotopologue()
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

/*
 * Selection Menu
 */

// // Transmute selection
// void DUQSpeciesDockWidget::transmuteSelection(int el)
// {
// 	// Check for valid Species
// 	Species* currentSpecies = selectedSpecies();
// 	if (currentSpecies == NULL) return;
// 
// 	// Make sure that an AtomType definition exists for the new element
// 	AtomType* at = GuiDUQ.atomTypeForElement(el);
// 	if (!at) GuiDUQ.addAtomType(el);
// 
// 	// Change Atoms
// 	for (RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms(); ri != NULL; ri = ri->next) currentSpecies->changeAtomElement(ri->item, el, at);
// 	currentSpecies->clearAtomSelection();
// 	
// 	// Update Species Isotopologues
// 	GuiDUQ.updateIsotopologues(currentSpecies);
// 
// 	// Update Dialogs
// 	refresh(
// }
// 
// // Change selected Atoms into Hydrogen
// void DUQSpeciesDockWidget::on_actionSelectionHydrogen_triggered(bool checked)
// {
// 	transmuteSelection(1);
// }
// 
// // Change selected Atoms into Carbon
// void DUQSpeciesDockWidget::on_actionSelectionCarbon_triggered(bool checked)
// {
// 	transmuteSelection(6);
// }
// 
// // Change selected Atoms into Nitrogen
// void DUQSpeciesDockWidget::on_actionSelectionNitrogen_triggered(bool checked)
// {
// 	transmuteSelection(7);
// }
// 
// // Change selected Atoms into Oxygen
// void DUQSpeciesDockWidget::on_actionSelectionOxygen_triggered(bool checked)
// {
// 	transmuteSelection(8);
// }
// 
// // Change selected Atoms into Silicon
// void DUQSpeciesDockWidget::on_actionSelectionSilicon_triggered(bool checked)
// {
// 	transmuteSelection(14);
// }
// 
// // Change selected Atoms into Phosphorous
// void DUQSpeciesDockWidget::on_actionSelectionPhosphorous_triggered(bool checked)
// {
// 	transmuteSelection(15);
// }
// 
// // Change selected Atoms into Sulfur
// void DUQSpeciesDockWidget::on_actionSelectionSulfur_triggered(bool checked)
// {
// 	transmuteSelection(16);
// }
// 
// // Change selected Atoms into another element
// void DUQSpeciesDockWidget::on_actionSelectionOther_triggered(bool checked)
// {
// 	dUQPeriodicTable ptable;
// 	int el = ptable.selectElement();
// 	if (el != -1) transmuteSelection(el);
// }
// 
// // Select same element as current
// void DUQSpeciesDockWidget::on_actionSelectionSelectSame_triggered(bool checked)
// {
// 	// Check for valid Species
// 	Species* currentSpecies = selectedSpecies();
// 	if (currentSpecies == NULL) return;
// 
// 	// Select all elements with same element as the first in the current Selection
// 	RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms();
// 	if (ri == NULL) return;
// 	int el = ri->item->element();
// 	
// 	for (Atom* i = currentSpecies->atoms(); i != NULL; i = i->next) if (i->element() == el) currentSpecies->selectAtom(i);
// 
// 	// Need to update view
// 	refresh(ViewEditGroup);
// }
// 
// // AtomType choice made in Selection menu
// void DUQSpeciesDockWidget::selectionAtomTypeChanged(bool checked)
// {
// 	// The text of the QAction contains the AtomTypes name
// 	QAction* action = static_cast<QAction*>(sender());
// 	if (!action) return;
// 	AtomType* at = GuiDUQ.findAtomType(qPrintable(action->text()));
// 	if (at == NULL)
// 	{
// 		printf("GUI_ERROR - QAction contains an invalid AtomType name (%s)\n", qPrintable(action->text()));
// 		return;
// 	}
// 
// 	// Get current Species
// 	Species* currentSpecies = selectedSpecies();
// 	if (!currentSpecies) return;
// 	
// 	// Set AtomTypes for Atoms
// 	for (RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms(); ri != NULL; ri = ri->next)
// 	{
// 		if (at->element() != ri->item->element())
// 		{
// 			msg.warn("AtomType '%s' cannot be assigned to Atom %i since their elements differ.\n", at->name(), ri->item->userIndex());
// 			continue;
// 		}
// 		ri->item->setAtomType(at);
// 	}
// 	
// 	// Update
// 	GuiDUQ.updateIsotopologues(currentSpecies);
// 	refresh(ViewEditGroup | IsotopologuesGroup);
// }

/*
// Slots - Species Group
*/

// Different Species selected in list
void DUQSpeciesDockWidget::on_DefinedSpeciesList_currentRowChanged(int row)
{
	if (refreshing_) return;
	refresh(65535);
}

// Item changed (i.e. text changed)
void DUQSpeciesDockWidget::on_DefinedSpeciesList_itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;

	currentSpecies->setName(dUQ_.uniqueSpeciesName(qPrintable(item->text())));

	// Update dependent data
	emit(dataChanged(DockWidgetPanel::SystemCompositionPanel));
}

// Add a new Species to the Model
void DUQSpeciesDockWidget::on_AddSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = dUQ_.addSpecies();
	currentSpecies->setName(dUQ_.uniqueSpeciesName("Unnamed Species", currentSpecies));

	// Refresh SpeciesTab and ExperimentTab
	refresh(65535);
	emit(dataChanged(DockWidgetPanel::SystemCompositionPanel));
}

// Remove the current Species from the Model
void DUQSpeciesDockWidget::on_RemoveSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	ui.SpeciesView->setSource(NULL);

	dUQ_.removeSpecies(currentSpecies);

	// Refresh SpeciesTab and ExperimentTab
	refresh(65535);
	emit(dataChanged(DockWidgetPanel::SystemCompositionPanel));
}

// Load Species data from a file
void DUQSpeciesDockWidget::on_LoadSpeciesButton_clicked(bool checked)
{
	// Create a file selector
	static QDir currentDir(getenv("PWD"));
	QStringList fileNames = QFileDialog::getOpenFileNames(this, "Open Model File", currentDir.path(), "XYZ Files (*.xyz)");
	
	// Was at least one file selected?
	if (fileNames.count() == 0) return;
	
	for (int n=0; n<fileNames.count(); ++n) dUQ_.loadSpecies(qPrintable(fileNames[n]));

	// Refresh SpeciesTab and ExperimentTab
	refresh(65535);
	emit(dataChanged(DockWidgetPanel::SystemCompositionPanel));

	// Select last loaded Species
	ui.DefinedSpeciesList->setCurrentRow(ui.DefinedSpeciesList->count()-1);
}

// Save Species definition to a file
void DUQSpeciesDockWidget::on_SaveSpeciesButton_clicked(bool checked)
{
	// TODO
}

/*
// Slots - Species Viewer
*/

// Add Bond from current Atom selection
void DUQSpeciesDockWidget::on_AddBondFromSelectionButton_clicked(bool checked)
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
	SpeciesAtom* atoms[2];
	for (int n=0; n<2; ++n) atoms[n] = currentSpecies->selectedAtom(n);
	currentSpecies->addBond(atoms[0], atoms[1]);
	
	// Clear selection and refresh display
	currentSpecies->clearAtomSelection();
	refresh(65535);
}

// Add Angle from current Atom selection
void DUQSpeciesDockWidget::on_AddAngleFromSelectionButton_clicked(bool checked)
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
	SpeciesAtom* atoms[3];
	for (int n=0; n<3; ++n) atoms[n] = currentSpecies->selectedAtom(n);
	currentSpecies->addAngle(atoms[0], atoms[1], atoms[2]);
	
	// Clear selection and refresh display
	currentSpecies->clearAtomSelection();
	refresh(65535);
}

// Add Grain from current Atom selection
void DUQSpeciesDockWidget::on_AddGrainFromSelectionButton_clicked(bool checked)
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
	SpeciesGrain* sg = currentSpecies->addGrain();
	for (RefListItem<SpeciesAtom,int>* ri = currentSpecies->selectedAtoms(); ri != NULL; ri = ri->next) currentSpecies->addAtomToGrain(ri->item, sg);
	sg->setName(currentSpecies->uniqueGrainName(sg->nameFromAtoms()));

	// Update the list of GrainDefinitions (removing empty ones etc.)
	currentSpecies->updateGrains();
	
	// Clear selection and refresh display
	currentSpecies->clearAtomSelection();
	refresh(65535);
}

// View AtomType Check clicked
void DUQSpeciesDockWidget::on_ViewAtomTypeCheck_clicked(bool checked)
{
	ui.SpeciesView->postRedisplay();
}

// View Index Check clicked
void DUQSpeciesDockWidget::on_ViewIndexCheck_clicked(bool checked)
{
	ui.SpeciesView->postRedisplay();
}

/*
 * Grains Group
 */

// Different grain selected in table
void DUQSpeciesDockWidget::on_GrainsTable_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
	if (refreshing_) return;
	refresh(65535);
}

// Grain information changed
void DUQSpeciesDockWidget::on_GrainsTable_cellChanged(int row, int column)
{
	if (refreshing_) return;
	// TODO
}

// Add new Grain to Species
void DUQSpeciesDockWidget::on_AddGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	SpeciesGrain* sg = currentSpecies->addGrain();
	sg->setName(currentSpecies->uniqueGrainName("NewGrain"));

	refresh(65535);
}

// Remove the current Grain from the Model
void DUQSpeciesDockWidget::on_RemoveGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	SpeciesGrain* sg = selectedGrainDefinition();
	
	currentSpecies->removeGrain(sg);
	
	// Update
	refresh(65535);
}

// Add default grain definition
void DUQSpeciesDockWidget::on_AddDefaultGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	if (currentSpecies->nGrains() != 0)
	{
		if (QMessageBox::question(this, "Add Default Grain", "All existing grains will be lost - proceed?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok) return;
	}
	
	currentSpecies->addDefaultGrain();

	// Update
	refresh(65535);
}

// Auto-add grains
void DUQSpeciesDockWidget::on_AutoGrainButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	if (currentSpecies->nGrains() != 0)
	{
		if (QMessageBox::question(this, "Auto Add Grains", "All existing grains will be lost - proceed?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok) return;
	}
	
	currentSpecies->autoAddGrains();
	
	// Update
	refresh(65535);
}

/*
 * Isotopologues Group
 */

// Determine Isotopologue and AtomType from QVariant data
bool DUQSpeciesDockWidget::getIsotopologueAndAtomType(QStringList data, Isotopologue*& iso, AtomType*& at)
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
void DUQSpeciesDockWidget::on_IsotopologuesTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (refreshing_) return;
// 	refresh(65535);
}

// Item changed (i.e. text changed)
void DUQSpeciesDockWidget::on_IsotopologuesTree_itemChanged(QTreeWidgetItem* item, int column)
{
	if (refreshing_) return;

	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = selectedIsotopologue();
	if (iso == NULL) return;
	
	// Check current text - if its the same, do nothing. If different, then set a (unique) version of it
	if (item->text(0) != iso->name()) iso->setName(currentSpecies->uniqueIsotopologueName(qPrintable(item->text(0)),iso));
	
	// Update
// 	refresh(DockWidgetPanel::SpeciesIsotopologuesPanel);
	emit(dataChanged(DockWidgetPanel::SampleIsotopologuesPanel));
}

// Add Isotopologue to Species
void DUQSpeciesDockWidget::on_AddIsotopologueToSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = currentSpecies->addIsotopologue("New Isotopologue");
	dUQ_.updateIsotopologues(currentSpecies, iso);

	// Update
	refresh(DockWidgetPanel::SpeciesIsotopologuesPanel);
	emit(dataChanged(DockWidgetPanel::SampleIsotopologuesPanel));
}

// Remove Isotopologue from Species
void DUQSpeciesDockWidget::on_RemoveIsotopologueFromSpeciesButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = selectedIsotopologue();
	if (iso == NULL) return;
	
	dUQ_.removeSpeciesIsotopologue(currentSpecies, iso);
	
	// Update
	refresh(DockWidgetPanel::SpeciesIsotopologuesPanel);
	emit(dataChanged(DockWidgetPanel::SampleIsotopologuesPanel));
}

// Copy Isotopologue
void DUQSpeciesDockWidget::on_CopyIsotopologueButton_clicked(bool checked)
{
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;
	
	Isotopologue* iso = selectedIsotopologue();
	if (iso == NULL) return;
	
	Isotopologue* newTope = currentSpecies->addIsotopologue(currentSpecies->uniqueIsotopologueName(iso->name()));
	dUQ_.updateIsotopologues(currentSpecies, newTope);
	for (RefListItem<AtomType,Isotope*>* ri = iso->isotopes(); ri != NULL; ri = ri->next) newTope->setAtomTypeIsotope(ri->item, ri->data);
	
	// Update
	refresh(DockWidgetPanel::SpeciesIsotopologuesPanel | DockWidgetPanel::SpeciesViewPanel);
	emit(dataChanged(DockWidgetPanel::SampleIsotopologuesPanel));
}

// Different Isotope selected for AtomType in Isotopologue
void DUQSpeciesDockWidget::isotopologueIsotopeChanged(int index)
{
	if (refreshing_) return;

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
	refresh(DockWidgetPanel::SpeciesViewPanel);
	emit(dataChanged(DockWidgetPanel::SampleIsotopologuesPanel));
}

