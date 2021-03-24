// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"
#include "modules/geomopt/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *GeometryOptimisationModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new GeometryOptimisationModuleWidget(parent, dissolve.processingModuleData(), this);
}
