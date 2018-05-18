/*
	*** RDF Module Widget - Functions
	*** src/modules/rdf/gui/modulewidget_funcs.cpp
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

#include "modules/rdf/gui/modulewidget.h"
#include "gui/uchroma/gui/uchromaview.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/duq.h"
#include "classes/atomtype.h"
#include "templates/variantpointer.h"
#include "templates/genericlisthelper.h"

// Constructor
RDFModuleWidget::RDFModuleWidget(QWidget* parent, Module* module, DUQ& dUQ) : ModuleWidget(parent), module_((RDFModule*) module), dUQ_(dUQ)
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
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::HalfVerticalShift);
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
	viewPane->collectionGroupManager().setVerticalShift(CollectionGroupManager::NoVerticalShift);
	viewPane->setAutoFollowType(ViewPane::AllAutoFollow);

	refreshing_ = false;
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
