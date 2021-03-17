// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calibration/calibration.h"
#include "modules/calibration/gui/modulewidget.h"

// Return a new widget controlling this Module
ModuleWidget *CalibrationModule::createWidget(QWidget *parent, Dissolve &dissolve)
{
    return new CalibrationModuleWidget(parent, this);
}
