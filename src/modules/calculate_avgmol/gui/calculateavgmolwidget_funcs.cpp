// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_avgmol/gui/calculateavgmolwidget.h"

CalculateAvgMolModuleWidget::CalculateAvgMolModuleWidget(QWidget *parent, CalculateAvgMolModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    module_->updateSpecies(dissolve_.processingModuleData());

    ui_.SpeciesView->setSpecies(&module_->averageSpecies());

    refreshing_ = false;
}
