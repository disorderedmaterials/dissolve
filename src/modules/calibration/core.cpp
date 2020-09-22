// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/calibration/calibration.h"

/*
 * Constructor / Destructor
 */

CalibrationModule::CalibrationModule() : Module()
{
    // Initialise Module - set up keywords etc.
    initialise();
}

CalibrationModule::~CalibrationModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *CalibrationModule::createInstance() const { return new CalibrationModule; }
