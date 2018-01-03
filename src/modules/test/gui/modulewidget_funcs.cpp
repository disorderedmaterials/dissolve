/*
	*** Test Module Widget - Functions
	*** src/modules/test/gui/modulewidget_funcs.cpp
	Copyright T. Youngs 2007-2018

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

	// Data Graph

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
	viewPane->axes().setTitle(0, "\\it{Q}, Angstroms\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);

	// Partial S(Q) Graph
	
	layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	partialSQGraph_ = new UChromaViewWidget;
	layout->addWidget(partialSQGraph_);
	ui.PartialSQPlotWidget->setLayout(layout);

	// Start a new, empty session
	partialSQGraph_->startNewSession(true);
	viewPane = partialSQGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{Q}, Angstroms\\sup{-1}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "S(Q)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);

	// Potentials Graph

	layout = new QVBoxLayout;
	layout->setContentsMargins(0,0,0,0);
	layout->setSpacing(4);
	potentialsGraph_ = new UChromaViewWidget;
	layout->addWidget(potentialsGraph_);
	ui.PotentialsPlotWidget->setLayout(layout);

	// Start a new, empty session
	potentialsGraph_->startNewSession(true);
	viewPane = potentialsGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, Angstroms");
	viewPane->axes().setMax(0, 10.0);
	// TODO Add escape sequence to add special character - e.g. \\sym{Delta}
	viewPane->axes().setTitle(1, "Delta \\it{r}, Delta phi \\it{r}");
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
	partialSQGraph_->refreshReferencedDataSets();

	dataGraph_->updateDisplay();
	partialSQGraph_->updateDisplay();
}

// Initialise controls
void TestModuleWidget::initialiseControls(TestModule* module)
{
	if (!module) return;

	CharString blockData;

	// Add reference data, calculated data to the dataGraph_
	RefListIterator<Data,bool> dataIterator(module->targetData());
	while (Data* data = dataIterator.iterate())
	{
		// Reference data
		blockData.sprintf("Collection '%s Exp'; Group '%s'; LineStyle 1.0 Solid; DataSet 'Reference'; Source XYData '%s'; EndDataSet; EndCollection", data->name(), data->name(), data->data().objectName());
		dataGraph_->addCollectionFromBlock(blockData);

		// Calculated data (total S(Q)) from associated module
		if (data->associatedModule())
		{
			blockData.sprintf("Collection '%s Calc'; Group '%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated'; Source XYData '%s//WeightedSQ//Total'; EndDataSet; EndCollection", data->name(), data->name(), data->associatedModule()->uniqueName());
			dataGraph_->addCollectionFromBlock(blockData);
		}
	}

	// Get pointer to PartialsModule which contains the unweighted S(Q) data
	Module* partialsModule = module_->dependentModule("Partials");
	CharString partialsModuleName = partialsModule ? partialsModule->uniqueName() : "Partials???";

	// Add experimentally-determined partial S(Q), calculated partial S(Q), and delta S(Q) to the partialSQGraph_
	const int nTypes = dUQ_.atomTypeList().nItems();
	int n = 0;
	for (AtomType* at1 = dUQ_.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			// Experimentally-determined unweighted partial
			blockData.sprintf("Collection '%s-%s Exp'; Group '%s-%s'; DataSet 'Experimental %s-%s'; Source XYData '%s//GeneratedSQ//%s-%s'; EndDataSet; EndCollection", at1->name(), at2->name(), at1->name(), at2->name(), at1->name(), at2->name(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);

			// Calculated partial
			blockData.sprintf("Collection '%s-%s Calc'; Group '%s-%s'; LineStyle 1.0 'Quarter Dash'; DataSet 'Calculated %s-%s'; Source XYData '%s//UnweightedSQ//%s-%s//Full'; EndDataSet; EndCollection", at1->name(), at2->name(), at1->name(), at2->name(), at1->name(), at2->name(), partialsModuleName.get(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);

			// Deltas
			blockData.sprintf("Collection '%s-%s Diff'; Group '%s-%s'; LineStyle 1.0 'Dots'; DataSet 'Delta %s-%s'; Source XYData '%s//DeltaSQ//%s-%s'; EndDataSet; EndCollection", at1->name(), at2->name(), at1->name(), at2->name(), at1->name(), at2->name(), module_->uniqueName(), at1->name(), at2->name());
			partialSQGraph_->addCollectionFromBlock(blockData);
		}
	}

	// Add delta g(r) and resulting pair potential additions to the potentialsGraph_
	n = 0;
	for (AtomType* at1 = dUQ_.atomTypeList().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			// Delta g(r)
			blockData.sprintf("Collection '%s-%s dg(r)'; Group '%s-%s'; DataSet '%s-%s Delta g(r)'; Source XYData '%s//DeltaGR//%s-%s'; EndDataSet; EndCollection", at1->name(), at2->name(), at1->name(), at2->name(), at1->name(), at2->name(), module_->uniqueName(), at1->name(), at2->name());
			potentialsGraph_->addCollectionFromBlock(blockData);

			// Generated potential
			blockData.sprintf("Collection '%s-%s dphi(r)'; Group '%s-%s'; LineStyle 1.0 'Quarter Dash'; DataSet '%s-%s dphi(r)'; Source XYData '%s//DeltaPhiR//%s-%s'; EndDataSet; EndCollection", at1->name(), at2->name(), at1->name(), at2->name(), at1->name(), at2->name(), module_->uniqueName(), at1->name(), at2->name());
			potentialsGraph_->addCollectionFromBlock(blockData);
		}
	}
}

// Disable sensitive controls within widget, ready for main code to run
void TestModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void TestModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool TestModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!dataGraph_->writeSession(parser)) return false;
	if (!partialSQGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool TestModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!dataGraph_->readSession(parser)) return false;
	if (!partialSQGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

