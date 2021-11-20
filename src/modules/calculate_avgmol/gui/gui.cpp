// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_avgmol/gui/calculateavgmolwidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateAvgMolModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateAvgMolModuleWidget(parent, this, dissolve);
}
