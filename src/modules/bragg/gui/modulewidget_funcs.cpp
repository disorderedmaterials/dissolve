/*
	*** Bragg Module Widget - Functions
	*** src/modules/bragg/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/bragg/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/duq.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
BraggModuleWidget::BraggModuleWidget(QWidget* parent, Module* module, DUQ& dUQ) : ModuleWidget(parent), module_((BraggModule*) module), dUQ_(dUQ)
{
	// Set up user interface
	ui.setupUi(this);

	// Grab our UChroma widget
	uChromaView_ = ui.PlotWidget;

	// Start a new, empty session
	uChromaView_->startNewSession(true);

	// Set up the view pane
	ViewPane* viewPane = uChromaView_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r) / S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);

	refreshing_ = false;
}

BraggModuleWidget::~BraggModuleWidget()
{
}

// Update controls within widget
void BraggModuleWidget::updateControls()
{
	// Ensure that any displayed data are up-to-date
	uChromaView_->refreshReferencedDataSets();

	uChromaView_->updateDisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void BraggModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void BraggModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool BraggModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget session
	if (!uChromaView_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool BraggModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget session
	if (!uChromaView_->readSession(parser)) return false;
	
	return true;
}

/*
 * Widgets / Functions
 */

