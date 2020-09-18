/*
    *** Procedure Editor Functions
    *** src/gui/procedureeditor_funcs.cpp
    Copyright T. Youngs 2012-2020

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

#include "base/lineparser.h"
#include "gui/charts/procedure.h"
#include "gui/gui.h"
#include "gui/procedureeditor.h"
#include "gui/widgets/mimetreewidgetitem.h"
#include "main/dissolve.h"
#include "templates/variantpointer.h"

ProcedureEditor::ProcedureEditor(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    procedure_ = nullptr;
    chart_ = nullptr;

    refreshing_ = false;
}

ProcedureEditor::~ProcedureEditor() {}

/*
 * Procedure Target
 */

// Set up to display / edit the target Procedure
void ProcedureEditor::setUp(Procedure *procedure, const CoreData &coreData)
{
    procedure_ = procedure;

    // Create the ProcedureChart and add it to the scroll area
    chart_ = new ProcedureChart(procedure, coreData);
    chart_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    ui_.ProcedureScrollArea->setWidget(chart_);
    ui_.ProcedureScrollArea->setWidgetResizable(true);
    ui_.ProcedureScrollArea->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    // Connect signals / slots
    connect(chart_, SIGNAL(dataModified()), this, SLOT(chartDataModified()));
    connect(chart_, SIGNAL(requiredSizeChanged()), this, SLOT(chartSizeChanged()));

    updateControls();
}

/*
 * Update
 */

// Update controls in tab
void ProcedureEditor::updateControls()
{
    refreshing_ = true;

    chart_->updateControls();

    refreshing_ = false;
}

// Disable sensitive controls within tab
void ProcedureEditor::disableSensitiveControls()
{
    // 	ui_.AvailableModulesTree->setEnabled(false);
    chart_->disableSensitiveControls();
}

// Enable sensitive controls within tab
void ProcedureEditor::enableSensitiveControls()
{
    // 	ui_.AvailableModulesTree->setEnabled(true);
    chart_->enableSensitiveControls();
}

/*
 * Widget Functions
 */

/*
 * State
 */

// Write widget state through specified LineParser
bool ProcedureEditor::writeState(LineParser &parser) const
{
    if (!chart_->writeState(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool ProcedureEditor::readState(LineParser &parser)
{
    if (!chart_->readState(parser))
        return false;

    return true;
}

/*
 * Signals / Slots
 */

// Data displayed in the chart has been modified
void ProcedureEditor::chartDataModified() { emit(dataModified()); }

// Required size of the chart widget has changed
void ProcedureEditor::chartSizeChanged() { ui_.ProcedureScrollArea->updateGeometry(); }
