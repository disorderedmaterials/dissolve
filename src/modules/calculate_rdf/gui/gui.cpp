// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_rdf/gui/modulewidget.h"
#include "modules/calculate_rdf/rdf.h"

// Return a new widget controlling this Module
ModuleWidget *CalculateRDFModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalculateRDFModuleWidget(parent, dissolve.processingModuleData(), this);
}
