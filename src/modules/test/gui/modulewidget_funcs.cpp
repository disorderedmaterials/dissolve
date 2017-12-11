/*
	*** Test Module Widget - Functions
	*** src/modules/test/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2007-2017

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

#include "modules/test/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/mimetreewidgetitem.h"
#include "main/duq.h"
#include "modules/test/test.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
TestModuleWidget::TestModuleWidget(QWidget* parent, Module* module, DUQ& dUQ) : ModuleWidget(parent), module_((TestModule*) module), dUQ_(dUQ)
{
	// Set up user interface
	ui.setupUi(this);

	// Add UChromaWidgets to the two PlotWidgets
	ViewPane* viewPane;
	QVBoxLayout* layout;

	layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	dataGraph_ = new UChromaViewWidget;
	layout->addWidget(dataGraph_);
	ui.DataPlotWidget->setLayout(layout);

	// Start a new, empty session
	dataGraph_->startNewSession(true);
	viewPane = dataGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, Angstroms");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r) / S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);

	layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	partialsGraph_ = new UChromaViewWidget;
	layout->addWidget(partialsGraph_);
	ui.PartialsPlotWidget->setLayout(layout);

	// Start a new, empty session
	partialsGraph_->startNewSession(true);
	viewPane = partialsGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, Angstroms");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r) / S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);

	initialiseControls(module_);

	refreshing_ = false;
}

TestModuleWidget::~TestModuleWidget()
{
}

// Update controls within widget
void TestModuleWidget::updateControls()
{
	// Ensure that any displayed data is up-to-date
	dataGraph_->refreshReferencedDataSets();
	partialsGraph_->refreshReferencedDataSets();

	dataGraph_->updateDisplay();
	partialsGraph_->updateDisplay();
}

// Initialise controls
void TestModuleWidget::initialiseControls(TestModule* module)
{
	if (!module) return;

	// Add reference data, calculated data, and difference to the dataGraph_
	RefListIterator<Data,bool> dataIterator(module->targetData());
	while (Data* data = dataIterator.iterate())
	{
		// Reference data
		CharString blockData("Collection '%s'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source XYData '%s'; EndDataSet; EndCollection", data->name(), data->name(), data->data().objectName());
		dataGraph_->addCollectionFromBlock(blockData);

		// Calculated data (total S(Q) from associated module
		if (data->associatedModule())
		{
			CharString blockData("Collection '%s'; Group '%s'; LineStyle 1.0 'Half Dash'; DataSet 'Calculated'; Source XYData '%s//WeightedSQ//Total'; EndDataSet; EndCollection", data->name(), data->associatedModule()->uniqueName());
			dataGraph_->addCollectionFromBlock(blockData);
		}
	}
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool TestModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!dataGraph_->writeSession(parser)) return false;
	if (!partialsGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool TestModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!dataGraph_->readSession(parser)) return false;
	if (!partialsGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

