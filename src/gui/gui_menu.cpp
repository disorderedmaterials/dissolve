/*
	*** Dissolve GUI - Menu Functions
	*** src/gui/gui_menu.cpp
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
#include "gui/addconfigurationdialog.h"
#include "gui/addforcefieldtermsdialog.h"
#include "gui/addprocessinglayerdialog.h"
#include "gui/importspeciesdialog.h"
#include "gui/modulecontrolwidget.h"
#include "gui/selectelementdialog.h"
#include "gui/selectspeciesdialog.h"
#include "gui/selectsystemtemplatedialog.h"
#include "gui/speciestab.h"
#include "main/dissolve.h"
#include "procedure/procedure.h"
#include "procedure/nodes/addspecies.h"
#include "procedure/nodes/box.h"
#include "procedure/nodes/parameters.h"
#include "templates/variantpointer.h"
#include <QDesktopServices>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

/*
 * Main Menu
 */

// Check whether current input needs to be saved and, if so, if it saved successfully
bool DissolveWindow::checkSaveCurrentInput()
{
	// First, check the modification status of the current session
	if (modified_)
	{
		QMessageBox queryBox;
		queryBox.setText("The current input file is unsaved.");
		queryBox.setInformativeText("Would you like to save it first?");
		queryBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		queryBox.setDefaultButton(QMessageBox::Cancel);
		int ret = queryBox.exec();

		if (ret == QMessageBox::Cancel) return false;
		else if (ret == QMessageBox::Yes)
		{
			// If an input filename has been set, just overwrite it. If not, request a name first
			if (!dissolve_.hasInputFilename())
			{
				QString newFile = QFileDialog::getSaveFileName(this, "Choose input file name to save", QDir().absolutePath(), "Dissolve input files (*.txt)");
				if (newFile.isEmpty()) return false;

				dissolve_.setInputFilename(qPrintable(newFile));
			}

			// Save the file
			if (!dissolve_.saveInput(dissolve_.inputFilename())) return false;

			modified_ = false;
		}
	}

	// All OK
	return true;
}

// Clear all data and start new simulation afresh
void DissolveWindow::startNew()
{
	// Clear any data-related tabs from the UI
	clearTabs();

	// Clear Dissolve itself
	dissolve_.clear();

	dissolveState_ = DissolveWindow::EditingState;
	localSimulation_ = true;
	modified_ = false;

	// Fully update GUI
	fullUpdate();

	// Make sure we are now on the Simulation stack page
	showMainStackPage(DissolveWindow::SimulationStackPage);
}

void DissolveWindow::on_FileNewAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	startNew();
}

void DissolveWindow::on_FileNewFromTemplateAction_triggered(bool checked)
{
	// Make sure there is nothing to save before we try to start afresh
	if (!checkSaveCurrentInput()) return;

	// Select the desired system template
	static SelectSystemTemplateDialog selectTemplateDialog(this, systemTemplates_);
	SystemTemplate* sysTemp = selectTemplateDialog.selectTemplate();
	if (!sysTemp) return;

	// Clear any data-related tabs from the UI
	clearTabs();

	// Load the input data
	if (!dissolve_.loadInputFromString(qPrintable(sysTemp->inputFileData())))
	{
		startNew();
		return;
	}

	localSimulation_ = true;

	// Fully update GUI
	fullUpdate();

	// Make sure we are now on the Simulation stack page
	showMainStackPage(DissolveWindow::SimulationStackPage);
}

void DissolveWindow::on_FileOpenLocalAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	// Request a new file to open
	QString inputFile = QFileDialog::getOpenFileName(this, "Choose input file to open", QDir().absolutePath(), "Dissolve input files (*.txt)");
	if (inputFile.isEmpty()) return;

	openLocalFile(qPrintable(inputFile), "", false, false);
}

void DissolveWindow::on_FileConnectAction_triggered(bool checked)
{
	// TODO
}

void DissolveWindow::on_FileOpenRecentAction_triggered(bool checked)
{
	// TODO
}

void DissolveWindow::on_FileCloseAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	// Make sure we are now on the Start stack page
	showMainStackPage(DissolveWindow::StartStackPage);
}

void DissolveWindow::on_FileSaveAction_triggered(bool checked)
{
	// If the file is not modified, nothing to do.
	if (!modified_) return;

	// If an input file name has not been set, get one now
	if (!dissolve_.hasInputFilename())
	{
		QString newFile = QFileDialog::getSaveFileName(this, "Choose input file name to save", QDir().absolutePath(), "Dissolve input files (*.txt)");
		if (newFile.isEmpty()) return;

		dissolve_.setInputFilename(qPrintable(newFile));
	}

	// Attempt to save the file
	if (!dissolve_.saveInput(dissolve_.inputFilename())) return;

	modified_ = false;

	updateControlsFrame();

	updateWindowTitle();
}

void DissolveWindow::on_FileSaveAsAction_triggered(bool checked)
{
	// Get a suitable input file name
	QString newFile = QFileDialog::getSaveFileName(this, "Choose input file name to save", QDir().absolutePath(), "Dissolve input files (*.txt)");
	if (newFile.isEmpty()) return;

	dissolve_.setInputFilename(qPrintable(newFile));

	// Attempt to save the file
	if (!dissolve_.saveInput(dissolve_.inputFilename())) return;

	modified_ = false;

	updateControlsFrame();

	updateWindowTitle();
}

void DissolveWindow::on_FileQuitAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	QCoreApplication::quit();
}

/*
 * Simulation
 */

void DissolveWindow::on_SimulationRunAction_triggered(bool checked)
{
	// Prepare the simulation
	if (!dissolve_.prepare()) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(-1);
}

void DissolveWindow::on_SimulationStepAction_triggered(bool checked)
{
	// Prepare the simulation
	if (!dissolve_.prepare()) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(1);
}

void DissolveWindow::on_SimulationStepFiveAction_triggered(bool checked)
{
	// Prepare the simulation
	if (!dissolve_.prepare()) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(5);
}

void DissolveWindow::on_SimulationPauseAction_triggered(bool checked)
{
	dissolveState_ = DissolveWindow::EditingState;

	emit stopIterating();

	// Update the controls
	updateControlsFrame();

	ui_.ControlPauseButton->setEnabled(false);
}

void DissolveWindow::on_SimulationSetRandomSeedAction_triggered(bool checked)
{
	// Create an input dialog to get the new seed
	bool ok;
	dissolve_.seed();
	int newSeed = QInputDialog::getInt(this, "Set random seed", "Enter the new value of the random seed, or -1 to remove set value", dissolve_.seed(), -1, 2147483647, 1, &ok);

	if (!ok) return;

	// Set and initialise random seed
	dissolve_.setSeed(newSeed);

	if (dissolve_.seed() == -1) srand( (unsigned)time( NULL ) );
	else srand(dissolve_.seed());
}

/*
 * Species
 */

void DissolveWindow::on_SpeciesCreateEmptyAction_triggered(bool checked)
{
	Species* newSpecies = dissolve_.addSpecies();

	setModified();
	fullUpdate();
	setCurrentTab(newSpecies);
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
	setCurrentTab(newSpecies);
}

void DissolveWindow::on_SpeciesImportDissolveAction_triggered(bool checked)
{
	static ImportSpeciesDialog addSpeciesDialog(this, dissolve_);

	addSpeciesDialog.reset();

	if (addSpeciesDialog.exec() == QDialog::Accepted)
	{
		Species* sp = addSpeciesDialog.importSpecies(dissolve_);

		// Fully update GUI
		setModified();
		fullUpdate();

		setCurrentTab(sp);
	}
}

void DissolveWindow::on_SpeciesRenameAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a SpeciesTab, then call its rename() function
	MainTab* tab = currentTab();
	if ((!tab) || (tab->type() != MainTab::SpeciesTabType)) return;
	tab->rename();
}

void DissolveWindow::on_SpeciesAddForcefieldTermsAction_triggered(bool checked)
{
	// Get the current Species (if a SpeciesTab is selected)
	Species* species = currentSpecies();
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

/*
 * Configuration
 */

void DissolveWindow::on_ConfigurationCreateEmptyAction_triggered(bool checked)
{
	Configuration* newConfiguration = dissolve_.addConfiguration();

	setModified();
	fullUpdate();
	setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateSimpleRandomMixAction_triggered(bool checked)
{
	// Create a SpeciesSelectDialog and use it to get the Species to add to the mix
	static SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

	RefList<Species> mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
	if (mixSpecies.nItems() == 0) return;

	// Create the Configuration and a suitable generator
	Configuration* newConfiguration = dissolve_.addConfiguration();
	Procedure& generator = newConfiguration->generator();
	ParametersProcedureNode* paramsNode = new ParametersProcedureNode;
	paramsNode->addParameter("rho", 0.1);
	generator.addRootSequenceNode(paramsNode);
	generator.addRootSequenceNode(new BoxProcedureNode);
	RefListIterator<Species> mixIterator(mixSpecies);
	while (Species* sp = mixIterator.iterate())
	{
		generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, 100, NodeValue("rho", paramsNode->parameterReferences())));
	}

	setModified();
	fullUpdate();
	setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationCreateRelativeRandomMixAction_triggered(bool checked)
{
	// Create a SpeciesSelectDialog and use it to get the Species to add to the mix
	static SelectSpeciesDialog speciesSelectDialog(this, dissolve_.coreData(), "Select species to mix");

	RefList<Species> mixSpecies = speciesSelectDialog.selectSpecies(1, -1);
	if (mixSpecies.nItems() == 0) return;

	// Create the Configuration and a suitable generator
	Configuration* newConfiguration = dissolve_.addConfiguration();
	Procedure& generator = newConfiguration->generator();
	ParametersProcedureNode* paramsNode = new ParametersProcedureNode;
	paramsNode->addParameter("populationA", 100);
	paramsNode->addParameter("rho", 0.1);
	generator.addRootSequenceNode(paramsNode);
	generator.addRootSequenceNode(new BoxProcedureNode);
	RefListIterator<Species> mixIterator(mixSpecies);
	int count = 0;
	while (Species* sp = mixIterator.iterate())
	{
		// Add a parameter for the ratio of this species to the first (or the population of the first)
		if (mixIterator.isFirst()) generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue("populationA", paramsNode->parameterReferences()), NodeValue("rho", paramsNode->parameterReferences())));
		else
		{
			CharString parameterName("ratio%c", 65+count);
			paramsNode->addParameter(parameterName, 1);

			generator.addRootSequenceNode(new AddSpeciesProcedureNode(sp, NodeValue(CharString("%s*populationA", parameterName.get()), paramsNode->parameterReferences()), NodeValue("rho", paramsNode->parameterReferences())));
		}

		++count;
	}

	setModified();
	fullUpdate();
	setCurrentTab(newConfiguration);
}

void DissolveWindow::on_ConfigurationRenameAction_triggered(bool checked)
{
	// Get the current tab - make sure it is a ConfigurationTab, then call its rename() function
	MainTab* tab = currentTab();
	if ((!tab) || (tab->type() != MainTab::ConfigurationTabType)) return;
	tab->rename();
}

/*
 * Layer
 */

void DissolveWindow::on_LayerAddAction_triggered(bool checked)
{
	static AddProcessingLayerDialog addProcessingLayerDialog(this, dissolve_);

	addProcessingLayerDialog.reset();

	if (addProcessingLayerDialog.exec() == QDialog::Accepted)
	{
		ModuleLayer* layer = addProcessingLayerDialog.importModuleLayer(dissolve_);

		// Fully update GUI
		setModified();
		fullUpdate();

		setCurrentTab(layer);
	}
}

/*
 * Workspace
 */

void DissolveWindow::on_WorkspaceAddNewAction_triggered(bool checked)
{
	// Add a new workspace
	bool ok;
	QString text = QInputDialog::getText(this, "New Workspace", "Enter the name of the new workspace", QLineEdit::Normal, "New Workspace", &ok);
	if (!ok || text.isEmpty()) return;

	MainTab* workspaceTab = addWorkspaceTab(qPrintable(text));

	setCurrentTab(workspaceTab);
}

/*
 * Help
 */

void DissolveWindow::on_HelpOnlineManualAction_triggered(bool checked)
{
	QDesktopServices::openUrl(QUrl("https://trisyoungs.github.io/dissolve"));
}

void DissolveWindow::on_HelpOnlineTutorialsAction_triggered(bool checked)
{
}
