/*
	*** Main Window - Menu Functions
	*** src/gui/mainwindow_menus.cpp
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
#include "classes/atomtype.h"
#include "classes/species.h"

/*
// File Menu
*/

// Load new input file
void MainWindow::on_actionFileLoad_triggered(bool checked)
{
	// Check to see if the current data has changed
	if (IS_MODIFIED)
	{
		if (QMessageBox::question(this, "Changes Will be Lost", "Current file is modified and unsaved - proceed with load?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok) return;
	}
	
	// Create a file selector
	static QDir currentDir(getenv("PWD"));
	QString fileName = QFileDialog::getOpenFileName(this, "Open Input File", currentDir.path(), "dUQ Files (*.not)");

	// Was a filename provided?
	if (fileName.isEmpty()) return;
	
	// Clear all current data and load new input
	dUQ_.clear();
	dUQ_.loadInput(qPrintable(fileName));

	// Return to SpeciesTab and update
	ui.MainTabs->setCurrentIndex(0);
	refreshSpeciesTab();
}

// Save current input file under existing filename
void MainWindow::on_actionFileSave_triggered(bool checked)
{
	QString fileName;

	// Has a filename already been assigned?
	if (dUQ_.hasFileName()) fileName = dUQ_.fileName();
	else
	{
		// Create a file selector
		static QDir currentDir(getenv("PWD"));
		fileName = QFileDialog::getSaveFileName(this, "Choose File to Save", currentDir.path(), "dUQ Files (*.not)");
	}

	// Do we now have a fileName to save to?
	if (fileName.isEmpty()) return;
	
	dUQ_.saveInput(qPrintable(fileName));
	updateTitle();
}

// Save current input under different filename
void MainWindow::on_actionFileSaveAs_triggered(bool checked)
{
	QString fileName;

	// Create a file selector
	static QDir currentDir(getenv("PWD"));
	fileName = QFileDialog::getSaveFileName(this, "Choose File to Save", currentDir.path(), "dUQ Files (*.not)");

	// Do we now have a fileName to save to?
	if (fileName.isEmpty()) return;
	
	dUQ_.saveInput(qPrintable(fileName));
	updateTitle();
}

// Quit program
void MainWindow::on_actionFileQuit_triggered(bool checked)
{
	QApplication::exit(0);
}

/*
// Help Menu
*/

// Show/Hide help frames
void MainWindow::setHelpFramesVisible(bool visible)
{
	ui.DefinedSpeciesHelpFrame->setVisible(visible);
	ui.ViewEditHelpFrame->setVisible(visible);
	ui.IntramolecularTermsHelpFrame->setVisible(visible);
	ui.GrainsHelpFrame->setVisible(visible);
	ui.IsotopologuesHelpFrame->setVisible(visible);
	ui.SystemCompositionHelpFrame->setVisible(visible);
	ui.SamplesHelpFrame->setVisible(visible);
	ui.AssignedIsotopologuesHelpFrame->setVisible(visible);
	ui.DefinedAtomTypesHelpFrame->setVisible(visible);
	ui.PairPotentialsHelpFrame->setVisible(visible);
}

void MainWindow::on_actionHelpShow_triggered(bool checked)
{
	setHelpFramesVisible(checked);
}

/*
// Tools Menu
*/

// void MainWindow::on_actionToolsCheckSetup_triggered(bool checked)
// {
// }

/*
// Selection Menu
*/

// Transmute selection
void MainWindow::transmuteSelection(int el)
{
	// Check for valid Species
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;

	// Make sure that an AtomType definition exists for the new element
	AtomType* at = dUQ_.atomTypeForElement(el);
	if (!at) dUQ_.addAtomType(el);

	// Change Atoms
	for (RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms(); ri != NULL; ri = ri->next) currentSpecies->changeAtomElement(ri->item, el, at);
	currentSpecies->clearAtomSelection();
	
	// Update Species Isotopologues
	dUQ_.updateIsotopologues(currentSpecies);

	// Need to update view
	refreshSpeciesTab(false, false, true, false, false);
}

// Change selected Atoms into Hydrogen
void MainWindow::on_actionSelectionHydrogen_triggered(bool checked)
{
	transmuteSelection(1);
}

// Change selected Atoms into Carbon
void MainWindow::on_actionSelectionCarbon_triggered(bool checked)
{
	transmuteSelection(6);
}

// Change selected Atoms into Nitrogen
void MainWindow::on_actionSelectionNitrogen_triggered(bool checked)
{
	transmuteSelection(7);
}

// Change selected Atoms into Oxygen
void MainWindow::on_actionSelectionOxygen_triggered(bool checked)
{
	transmuteSelection(8);
}

// Change selected Atoms into Silicon
void MainWindow::on_actionSelectionSilicon_triggered(bool checked)
{
	transmuteSelection(14);
}

// Change selected Atoms into Phosphorous
void MainWindow::on_actionSelectionPhosphorous_triggered(bool checked)
{
	transmuteSelection(15);
}

// Change selected Atoms into Sulfur
void MainWindow::on_actionSelectionSulfur_triggered(bool checked)
{
	transmuteSelection(16);
}

// Change selected Atoms into another element
void MainWindow::on_actionSelectionOther_triggered(bool checked)
{
	dUQPeriodicTable ptable;
	int el = ptable.selectElement();
	if (el != -1) transmuteSelection(el);
}

// Select same element as current
void MainWindow::on_actionSelectionSelectSame_triggered(bool checked)
{
	// Check for valid Species
	Species* currentSpecies = selectedSpecies();
	if (currentSpecies == NULL) return;

	// Select all elements with same element as the first in the current Selection
	RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms();
	if (ri == NULL) return;
	int el = ri->item->element();
	
	for (Atom* i = currentSpecies->atoms(); i != NULL; i = i->next) if (i->element() == el) currentSpecies->selectAtom(i);

	// Need to update view
	refresh(ViewEditGroup);
}

// AtomType choice made in Selection menu
void MainWindow::selectionAtomTypeChanged(bool checked)
{
	// The text of the QAction contains the AtomTypes name
	QAction* action = static_cast<QAction*>(sender());
	if (!action) return;
	AtomType* at = dUQ_.findAtomType(qPrintable(action->text()));
	if (at == NULL)
	{
		printf("GUI_ERROR - QAction contains an invalid AtomType name (%s)\n", qPrintable(action->text()));
		return;
	}

	// Get current Species
	Species* currentSpecies = selectedSpecies();
	if (!currentSpecies) return;
	
	// Set AtomTypes for Atoms
	for (RefListItem<Atom,int>* ri = currentSpecies->selectedAtoms(); ri != NULL; ri = ri->next)
	{
		if (at->element() != ri->item->element())
		{
			msg.warn("AtomType '%s' cannot be assigned to Atom %i since their elements differ.\n", at->name(), ri->item->userIndex());
			continue;
		}
		ri->item->setAtomType(at);
	}
	
	// Update
	dUQ_.updateIsotopologues(currentSpecies);
	refresh(ViewEditGroup | IsotopologuesGroup);
}
