// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/rdf/gui/modulewidget.h"
#include "modules/rdf/rdf.h"

// Return a new widget controlling this Module
ModuleWidget *RDFModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new RDFModuleWidget(parent, dissolve.processingModuleData(), this, dissolve);
}
