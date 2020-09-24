// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/rdf/gui/modulewidget.h"
#include "modules/rdf/rdf.h"

// Return a new widget controlling this Module
ModuleWidget *RDFModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new RDFModuleWidget(parent, this, dissolve);
}
