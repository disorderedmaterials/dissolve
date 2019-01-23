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
#include "gui/addprocessinglayerdialog.h"
#include "gui/addspeciesdialog.h"
#include "gui/modulecontrolwidget.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

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

void DissolveWindow::on_SessionNewAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	startNew();
}

void DissolveWindow::on_SessionSetupWizardAction_triggered(bool checked)
{
}

void DissolveWindow::on_SessionOpenLocalAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	// Request a new file to open
	QString inputFile = QFileDialog::getOpenFileName(this, "Choose input file to open", QDir().absolutePath(), "Dissolve input files (*.txt)");
	if (inputFile.isEmpty()) return;

	// Clear any data-related tabs from the UI
	clearTabs();

	// Clear Dissolve itself
	dissolve_.clear();

	// Load the input file
	if (!dissolve_.loadInput(qPrintable(inputFile)))
	{
		startNew();
		return;
	}

	localSimulation_ = true;

	// Load restart file if it exists
	CharString restartFile("%s.restart", qPrintable(inputFile));
	if (DissolveSys::fileExists(restartFile))
	{
		Messenger::print("\nRestart file '%s' exists and will be loaded.\n", restartFile.get());
		if (!dissolve_.loadRestart(restartFile.get()))
		{
			Messenger::error("Restart file contained errors.\n");
			startNew();
			return;
		}
	}
	else Messenger::print("\nRestart file '%s' does not exist.\n", restartFile.get());

	dissolveState_ = EditingState;

	// Check the beat file
	CharString beatFile("%s.bet", qPrintable(inputFile));
	if (DissolveSys::fileExists(beatFile))
	{
// 		if (
	}

	// Fully update GUI
	fullUpdate();

	// Make sure we are now on the Simulation stack page
	showMainStackPage(DissolveWindow::SimulationStackPage);
}

void DissolveWindow::on_SessionOpenRemoteAction_triggered(bool checked)
{
	// TODO
}

void DissolveWindow::on_SessionOpenRecentAction_triggered(bool checked)
{
	// TODO
}

void DissolveWindow::on_SessionCloseAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	startNew();
}

void DissolveWindow::on_SessionSaveAction_triggered(bool checked)
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

	updateWindowTitle();
}

void DissolveWindow::on_SessionSaveAsAction_triggered(bool checked)
{
}

void DissolveWindow::on_SessionQuitAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	QCoreApplication::quit();
}

/*
 * Simulation
 */

void DissolveWindow::on_SimulationAddSpeciesAction_triggered(bool checked)
{
	static AddSpeciesDialog addSpeciesDialog(this, dissolve_);

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

void DissolveWindow::on_SimulationAddConfigurationAction_triggered(bool checked)
{
	static AddConfigurationDialog addConfigurationDialog(this, dissolve_);

	addConfigurationDialog.reset();

	if (addConfigurationDialog.exec() == QDialog::Accepted)
	{
		Configuration* cfg = addConfigurationDialog.importConfiguration(dissolve_);

		// Fully update GUI
		setModified();
		fullUpdate();

		setCurrentTab(cfg);
	}
}

void DissolveWindow::on_SimulationAddProcessingLayerAction_triggered(bool checked)
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
 * Control
 */

void DissolveWindow::on_SimulationRunAction_triggered(bool checked)
{
	// Make sure everything is set-up
	if ((!dissolve_.isSetUp()) && (!dissolve_.setUp())) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(-1);
}

void DissolveWindow::on_SimulationStepAction_triggered(bool checked)
{
	// Make sure everything is set-up
	if ((!dissolve_.isSetUp()) && (!dissolve_.setUp())) return;

	// Prepare the GUI
	setWidgetsForRun();

	dissolveState_ = DissolveWindow::RunningState;

	// Update the controls
	updateControlsFrame();

	emit iterate(1);
}

void DissolveWindow::on_SimulationStepFiveAction_triggered(bool checked)
{
	// Make sure everything is set-up
	if ((!dissolve_.isSetUp()) && (!dissolve_.setUp())) return;

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

	ui.ControlPauseButton->setEnabled(false);
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

void DissolveWindow::on_HelpViewQuickStartGuideAction_triggered(bool checked)
{
	if (!checkSaveCurrentInput()) return;

	startNew();

	// Reset the guide wizard widget and set up the QuickStart guide in it
	ui.GuideWidget->clear();
	ui.GuideWidget->setContent(":/quickstart/guides/quickstart/quickstart.xml");
	ui.GuideWidget->setVisible(true);
}

void DissolveWindow::on_HelpRunATutorialAction_triggered(bool checked)
{
}
