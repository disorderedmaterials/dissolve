// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_avgmol/gui/modulewidget.h"

CalculateAvgMolModuleWidget::CalculateAvgMolModuleWidget(QWidget *parent, const GenericList &processingData,
                                                         CalculateAvgMolModule *module)
    : ModuleWidget(parent, processingData), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    ui_.SpeciesView->setSpecies(&module_->averageSpecies());

    refreshing_ = false;
}

/*
 * UI
 */

// Update controls within widget
void CalculateAvgMolModuleWidget::updateControls(ModuleWidget::UpdateType updateType) {}

/*
 * Widgets / Functions
 */
