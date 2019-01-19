/*
	*** uChroma Full Widget - Functions 
	*** src/gui/uchroma/gui/uchromafull_funcs.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/uchromafull.h"
#include "gui/uchroma/render/fontinstance.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"
#include <QMessageBox>
#include <QSettings>

// Constructor
UChromaFullWindow::UChromaFullWindow(QMainWindow *parent) : QMainWindow(parent), UChromaBase(this), viewTypeActionGroup_(this)
{
	// Initialise the icon resource
	Q_INIT_RESOURCE(icons);

	// Call the main creation function
	ui.setupUi(this);

	// Set Viewer pointer
	viewer_ = ui.MainView;

	// Set variable defaults
	interactionMode_ = UChromaBase::ViewInteraction;
	interactionAxis_ = -1;
	interactionStarted_ = false;
	clickedInteractionValue_ = 0.0;
	clickedObject_ = UChromaViewer::NoObject;
	currentInteractionValue_ = 0.0;
	refreshing_ = false;

	// Load settings...
	loadSettings();

	// Set UChroma pointers in widgets/dialogs where necessary
	ui.MainView->setUChromaBase(this);

	// Hide LeftWidgets (Collection list etc.) initially
	ui.LeftWidgetsWidget->setVisible(false);

	// Connect CollectionTree context menu signal
	connect(ui.CollectionTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(collectionTreeContextMenuRequested(QPoint)));

	// Connect sub-window closed signal to toggle buttons / menu items in uChroma's main window
	connect(&axesWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowAxes, SLOT(setChecked(bool)));
	connect(&dataWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowData, SLOT(setChecked(bool)));
	connect(&styleWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowStyle, SLOT(setChecked(bool)));
	connect(&transformWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowTransform, SLOT(setChecked(bool)));
	connect(&viewWindow_, SIGNAL(windowClosed(bool)), ui.actionWindowView, SLOT(setChecked(bool)));

	// Create necessary action groups
	// -- ViewType actions
	viewTypeActionGroup_.addAction(ui.actionViewNormal);
	viewTypeActionGroup_.addAction(ui.actionViewAutoStretched3D);
	viewTypeActionGroup_.addAction(ui.actionViewFlatXY);
	viewTypeActionGroup_.addAction(ui.actionViewFlatXZ);
	viewTypeActionGroup_.addAction(ui.actionViewFlatZY);
	viewTypeActionGroup_.addAction(ui.actionViewLinked);
	// -- Interaction mode actions
	QActionGroup* actionGroup = new QActionGroup(this);
	actionGroup->addAction(ui.actionInteractNone);
	actionGroup->addAction(ui.actionInteractX);
	actionGroup->addAction(ui.actionInteractY);
	actionGroup->addAction(ui.actionInteractZ);

	// Add QLabel as a normal widget to the status bar
	statusBarInfoLabel_ = new QLabel(this);
	statusBarInfoLabel_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	ui.StatusBar->addWidget(statusBarInfoLabel_);
	statusBarRenderingTimeLabel_ = new QLabel(this);
	ui.StatusBar->addPermanentWidget(statusBarRenderingTimeLabel_);	

	// Set initial interaction mode
	setInteractionMode(UChromaBase::ViewInteraction, -1);
}

// Destructor
UChromaFullWindow::~UChromaFullWindow()
{
}

/*
 * Window Functions
 */

// Return centre coordinate of main window
QPoint UChromaFullWindow::centrePos()
{
	QPoint centre = pos();
	centre += QPoint(width()/2, height()/2);
	return centre;
}

// Load settings
void UChromaFullWindow::loadSettings()
{
	QSettings settings;

	// Viewer font
	if (settings.contains("ViewerFont")) setViewerFontFileName(qPrintable(settings.value("ViewerFont").toString()));
}

// Save settings
void UChromaFullWindow::saveSettings()
{
	QSettings settings;

	// Viewer font
	settings.setValue("ViewerFont", viewerFontFileName());
}
