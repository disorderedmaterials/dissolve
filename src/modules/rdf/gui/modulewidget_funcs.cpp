/*
	*** RDF Module Widget - Functions
	*** src/modules/rdf/gui/modulewidget_funcs.cpp
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

#include "modules/rdf/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
RDFModuleWidget::RDFModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve) : ModuleWidget(parent), module_((RDFModule*) module), dissolve_(dissolve)
{
	// Set up user interface
	ui.setupUi(this);

	ViewPane* viewPane;

	// Set up partial g(r) graph

	partialsGraph_ = ui.PartialsPlotWidget;

	partialsGraph_->startNewSession(true);
	viewPane = partialsGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::TwoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	// Set up total G(r) graph

	totalsGraph_ = ui.TotalsPlotWidget;

	totalsGraph_->startNewSession(true);
	viewPane = totalsGraph_->currentViewPane();
	viewPane->setViewType(ViewPane::FlatXYView);
	viewPane->axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	viewPane->axes().setMax(0, 10.0);
	viewPane->axes().setTitle(1, "g(r)");
	viewPane->axes().setMin(1, -1.0);
	viewPane->axes().setMax(1, 1.0);
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::OneVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	refreshing_ = false;

	currentConfiguration_ = NULL;

	setGraphDataTargets(module_);
}

RDFModuleWidget::~RDFModuleWidget()
{
}

// Update controls within widget
void RDFModuleWidget::updateControls()
{
	// Ensure that any displayed data are up-to-date
	partialsGraph_->refreshReferencedDataSets();
	totalsGraph_->refreshReferencedDataSets();

	partialsGraph_->updateDisplay();
	totalsGraph_->updateDisplay();
}

// Disable sensitive controls within widget, ready for main code to run
void RDFModuleWidget::disableSensitiveControls()
{
}

// Enable sensitive controls within widget, ready for main code to run
void RDFModuleWidget::enableSensitiveControls()
{
}

/*
 * ModuleWidget Implementations
 */

// Write widget state through specified LineParser
bool RDFModuleWidget::writeState(LineParser& parser)
{
	// Write UChromaWidget sessions
	if (!partialsGraph_->writeSession(parser)) return false;
	if (!totalsGraph_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool RDFModuleWidget::readState(LineParser& parser)
{
	// Read UChromaWidget sessions
	if (!partialsGraph_->readSession(parser)) return false;
	if (!totalsGraph_->readSession(parser)) return false;

	return true;
}

/*
 * Widgets / Functions
 */

// Set data targets in graphs
void RDFModuleWidget::setGraphDataTargets(RDFModule* module)
{
	if (!module) return;

	// Add Configuration targets to the combo box
	ui.TargetCombo->clear();
	RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
	while (Configuration* config = configIterator.iterate()) ui.TargetCombo->addItem(config->name(), VariantPointer<Configuration>(config));

	// Loop over Configurations and add total G(R)
	CharString blockData;
	configIterator.restart();
	while (Configuration* cfg = configIterator.iterate())
	{
		// Add calculated total G(r)
		blockData.sprintf("Collection '%s'; Group 'Calc'; DataSet 'Calculated'; Source Data1D '%s//UnweightedGR//Total'; EndDataSet; EndCollection", cfg->niceName(), cfg->niceName());
		totalsGraph_->addCollectionFromBlock(blockData);
	}
}

void RDFModuleWidget::on_TargetCombo_currentIndexChanged(int index)
{
	// Remove any current collections
	partialsGraph_->clearCollections();

	// Get target Configuration
	currentConfiguration_ = (Configuration*) VariantPointer<Configuration>(ui.TargetCombo->itemData(index));
	if (!currentConfiguration_) return;

	CharString blockData;
	const AtomTypeList cfgTypes = currentConfiguration_->usedAtomTypesList();
	int n = 0;
	for (AtomType* at1 = dissolve_.atomTypes().first(); at1 != NULL; at1 = at1->next, ++n)
	{
		int m = n;
		for (AtomType* at2 = at1; at2 != NULL; at2 = at2->next, ++m)
		{
			CharString id("%s-%s", at1->name(), at2->name());

			// Full partial
			blockData.sprintf("Collection '%s'; Group '%s'; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedGR//%s-%s//Full'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), currentConfiguration_->niceName(), at1->name(), at2->name());
			partialsGraph_->addCollectionFromBlock(blockData);

			// Bound partial
			blockData.sprintf("Collection '%s Bound'; Group '%s'; LineStyle 1.0 'Half Dash'; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedGR//%s-%s//Bound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), currentConfiguration_->niceName(), at1->name(), at2->name());
			partialsGraph_->addCollectionFromBlock(blockData);

			// Unbound partial
			blockData.sprintf("Collection '%s Unbound'; Group '%s'; LineStyle 1.0 Dots; DataSet 'Calculated %s'; Source Data1D '%s//UnweightedGR//%s-%s//Unbound'; EndDataSet; EndCollection", id.get(), id.get(), id.get(), currentConfiguration_->niceName(), at1->name(), at2->name());
			partialsGraph_->addCollectionFromBlock(blockData);
		}
	}
}
