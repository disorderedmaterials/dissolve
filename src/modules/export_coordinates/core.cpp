// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/export_coordinates/exportcoords.h"

ExportCoordinatesModule::ExportCoordinatesModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *ExportCoordinatesModule::createInstance() const { return new ExportCoordinatesModule; }
