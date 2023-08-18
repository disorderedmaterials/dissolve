// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/avgMol/avgMol.h"
#include "modules/avgMol/gui/avgMolWidget.h"

AvgMolModuleWidget::AvgMolModuleWidget(QWidget *parent, AvgMolModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    module_->updateSpecies(dissolve_.processingModuleData());

    ui_.SpeciesView->setSpecies(&module_->averageSpecies());

    // Hide site actions in the SpeciesViewer
    ui_.SpeciesView->setSiteActionsVisible(false);

    refreshing_ = false;
}
