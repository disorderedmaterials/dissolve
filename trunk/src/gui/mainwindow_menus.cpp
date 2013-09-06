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

#include "main/flags.h"
#include "gui/mainwindow.h"
#include "classes/atomtype.h"
#include "classes/species.h"

/*
// File Menu
*/

// Start new input file
void MainWindow::on_actionFileNew_triggered(bool checked)
{
	if (Flags::modificationFlag())
	{
		if (QMessageBox::question(this, "Changes Will be Lost", "Current file is modified and unsaved - conontiue without saving?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok) return;
	}

	dUQ_.clear();
	refresh(65535);
}

// Load new input file
void MainWindow::on_actionFileLoad_triggered(bool checked)
{
	// Check to see if the current data has changed
	if (Flags::modificationFlag())
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
	refresh(65535);
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
 * Windows Menu
 */

void MainWindow::on_actionWindowsAtomTypes_triggered(bool checked)
{
	atomTypesDockWidget_.show();
	atomTypesDockWidget_.raise();
}

void MainWindow::on_actionWindowsFQ_triggered(bool checked)
{
	fqDockWidget_.show();
	fqDockWidget_.raise();
}

void MainWindow::on_actionWindowsSamples_triggered(bool checked)
{
	samplesDockWidget_.show();
	samplesDockWidget_.raise();
}

void MainWindow::on_actionWindowsSpecies_triggered(bool checked)
{
	speciesDockWidget_.show();
	speciesDockWidget_.raise();
}

void MainWindow::on_actionWindowsSQ_triggered(bool checked)
{
	sqDockWidget_.show();
	sqDockWidget_.raise();
}

// Show system dialog
void MainWindow::on_actionWindowsSystem_triggered(bool checked)
{
	systemDockWidget_.show();
	systemDockWidget_.raise();
}

void MainWindow::on_actionWindowsCheckSetup_triggered(bool checked)
{
	dUQ_.setupSimulation();
}
