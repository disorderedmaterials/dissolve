/*
	*** Dissolve GUI - Species Menu Functions
	*** src/gui/menu_species.cpp
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

#include "gui/gui.h"
#include "gui/speciestab.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/importspeciesdialog.h"
#include "gui/selectelementdialog.h"
#include "classes/species.h"
#include <QFileDialog>

void DissolveWindow::on_SpeciesCreateEmptyAction_triggered(bool checked)
{
	Species* newSpecies = dissolve_.addSpecies();

	setModified();
	fullUpdate();
	ui_.MainTabs->setCurrentTab(newSpecies);
}

void DissolveWindow::on_SpeciesCreateAtomicAction_triggered(bool checked)
{
	// Raise an element selection dialog
	static SelectElementDialog selectElementDialog(this);
	Element* el = selectElementDialog.selectElement();
	if (!el) return;

	// Create the new Species, and add a single atom at {0,0,0}
	Species* newSpecies = dissolve_.addSpecies();
	newSpecies->addAtom(el, Vec3<double>());
	newSpecies->setName(dissolve_.coreData().uniqueSpeciesName(el->symbol()));

	setModified();
	fullUpdate();
	ui_.MainTabs->setCurrentTab(newSpecies);
}

void DissolveWindow::on_SpeciesImportFromDissolveAction_triggered(bool checked)
{
	static ImportSpeciesDialog addSpeciesDialog(this, dissolve_);

	addSpeciesDialog.reset();

	if (addSpeciesDialog.exec() == QDialog::Accepted)
	{
		Species* sp = addSpeciesDialog.importSpecies(dissolve_);

		// Fully update GUI
		setModified();
		fullUpdate();

		ui_.MainTabs->setCurrentTab(sp);
	}
}

void DissolveWindow::on_SpeciesImportFromXYZAction_triggered(bool checked)
{
	// Request a new file to open
	QString xyzFile = QFileDialog::getOpenFileName(this, "Choose XYZ file to open", QDir().absolutePath(), "XYZ Coordinates (*.xyz)");
	if (xyzFile.isEmpty()) return;

	// Add new species, load from the xyz, and create intramolecular terms
	Species* sp = dissolve_.addSpecies();
	sp->loadFromXYZ(qPrintable(xyzFile));
	sp->addMissingBonds();

	// Fully update GUI
	setModified();
	fullUpdate();

	ui_.MainTabs->setCurrentTab(sp);
}

void DissolveWindow::on_SpeciesRenameAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a SpeciesTab, then call its rename() function
	MainTab* tab = ui_.MainTabs->currentTab();
	if ((!tab) || (tab->type() != MainTab::SpeciesTabType)) return;
	tab->rename();
}

void DissolveWindow::on_SpeciesAddForcefieldTermsAction_triggered(bool checked)
{
	// Get the current Species (if a SpeciesTab is selected)
	Species* species = ui_.MainTabs->currentSpecies();
	if (!species) return;

	static AddForcefieldTermsDialog addForcefieldTermsDialog(this, dissolve_);

	addForcefieldTermsDialog.reset();
	addForcefieldTermsDialog.setTargetSpecies(species);

	if (addForcefieldTermsDialog.exec() == QDialog::Accepted)
	{
		addForcefieldTermsDialog.applyForcefieldTerms(dissolve_);

		// Fully update GUI
		setModified();
		fullUpdate();
	}
}

void DissolveWindow::on_SpeciesDeleteAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a SpeciesTab
	MainTab* tab = ui_.MainTabs->currentTab();
	if ((!tab) || (tab->type() != MainTab::SpeciesTabType)) return;

	// Cast up the tab to a SpeciesTab
	SpeciesTab* spTab = dynamic_cast<SpeciesTab*>(tab);
	if (!spTab) return;

	// Check that we really want to delete the Species
	if (!spTab->close()) return;

	// Update the GUI
	ui_.MainTabs->removeByPage(spTab->page());
	setModified();
	fullUpdate();
}
