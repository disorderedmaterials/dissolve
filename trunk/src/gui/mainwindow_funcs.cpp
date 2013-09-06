/*
	*** Main Window Functions
	*** src/gui/mainwindow_funcs.cpp
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
#include "gui/led.uih"
#include "base/messenger.h"
#include "version.h"

// Constructor
MainWindow::MainWindow(DUQObject& dUQ) : QMainWindow(NULL), dUQ_(dUQ),
	periodicTable_(this), atomTypesDockWidget_(this, dUQ), fqDockWidget_(this, dUQ), samplesDockWidget_(this, dUQ), speciesDockWidget_(this, dUQ), sqDockWidget_(this, dUQ), systemDockWidget_(this, dUQ)
{
	// Call the main creation function
	ui.setupUi(this);
	
	// Link MainViewer to the main DUQ configuration
	ui.MainViewer->setSource(dUQ_.configuration());
	dUQ_.setMainWindow(this);

	// Hide / Assign Dock Widgets
	atomTypesDockWidget_.hide();
	addDockWidget(Qt::LeftDockWidgetArea, &atomTypesDockWidget_);
	fqDockWidget_.hide();
	addDockWidget(Qt::RightDockWidgetArea, &fqDockWidget_);
	samplesDockWidget_.hide();
	addDockWidget(Qt::RightDockWidgetArea, &samplesDockWidget_);
	speciesDockWidget_.hide();
	addDockWidget(Qt::RightDockWidgetArea, &speciesDockWidget_);
	sqDockWidget_.hide();
	addDockWidget(Qt::RightDockWidgetArea, &sqDockWidget_);
	systemDockWidget_.hide();
	addDockWidget(Qt::RightDockWidgetArea, &systemDockWidget_);

	// Connect 'dataChanged' signals of dock widgets
	connect(&atomTypesDockWidget_, SIGNAL(dataChanged(int)), this, SLOT(refresh(int)));

	// Link 
// 	ui.SpeciesViewer->setAtomContextMenu(ui.menuSelection);
// 	ui.SpeciesViewer->setSpeciesUpdateTargets(ViewEditGroup);

	// Move dUQ execution to its own thread and connect various signals
// 	dUQ_.moveToThread(&simulationThread_);
// 	connect(&dUQ_, SIGNAL(simulationFinished(int)), SLOT(simulationFinished(int)));
// 	dUQ_.connect(&simulationThread_, SIGNAL(started()), SLOT(startSimulation()));
// 	connect(&dUQ_, SIGNAL(sendDUQSignal(int,int)), SLOT(receiveDUQSignal(int,int)));
	
// 	ui.statusbar->addPermanentWidget(new LED(this));
	
	// Connect the OutputHandler displayText signal to the QTextEdit, and link it into the main Messenger
	connect(&outputHandler_, SIGNAL(displayText(QString)), ui.MessagesEdit, SLOT(append(QString)), Qt::QueuedConnection);
	msg.setTargetOutputHandler(&outputHandler_);
}

// Destructor
MainWindow::~MainWindow()
{
}

// Window close event
void MainWindow::closeEvent(QCloseEvent* event)
{
	if (Flags::modificationFlag())
	{
		if (QMessageBox::question(this, "Changes Will be Lost", "Current file is modified and unsaved - really quit?", QMessageBox::Cancel|QMessageBox::Ok, QMessageBox::Cancel) != QMessageBox::Ok)
		{
			event->ignore();
			return;
		}
	}
}

// General refresh function
void MainWindow::refresh(int targets)
{
	// Update the main window title
	updateTitle();
	
	atomTypesDockWidget_.refresh(targets);
	fqDockWidget_.refresh(targets);
	samplesDockWidget_.refresh(targets);
	speciesDockWidget_.refresh(targets);
	sqDockWidget_.refresh(targets);
	systemDockWidget_.refresh(targets);
}

// Update main window title
void MainWindow::updateTitle()
{
	Dnchar title;
	title.sprintf("dUQ r%s - '%s'", DUQREVISION, dUQ_.fileName());
	if (Flags::modificationFlag()) title.strcat(" (modified)");
	setWindowTitle(title.get());
}
