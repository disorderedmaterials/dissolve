// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
void ProcedureEditor::disableSensitiveControls() { chart_->disableSensitiveControls(); }

// Enable sensitive controls within tab
void ProcedureEditor::enableSensitiveControls() { chart_->enableSensitiveControls(); }

/*
 * Signals / Slots
 */

// Data displayed in the chart has been modified
void ProcedureEditor::chartDataModified() { emit(dataModified()); }

// Required size of the chart widget has changed
void ProcedureEditor::chartSizeChanged() { ui_.ProcedureScrollArea->updateGeometry(); }
