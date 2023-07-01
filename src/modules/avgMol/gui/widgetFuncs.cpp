// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/avgmol/avgmol.h"
#include "modules/avgmol/gui/avgmolwidget.h"

AvgMolModuleWidget::AvgMolModuleWidget(QWidget *parent, AvgMolModule *module, Dissolve &dissolve)
    : ModuleWidget(parent, dissolve), module_(module)
{
    // Set up user interface
    ui_.setupUi(this);

    module_->updateSpecies(dissolve_.processingModuleData());

    ui_.SpeciesView->setSpecies(&module_->averageSpecies());

    refreshing_ = false;
}
