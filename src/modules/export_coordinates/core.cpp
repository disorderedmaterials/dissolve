// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/export_coordinates/exportcoords.h"

/*
 * Constructor / Destructor
 */

ExportCoordinatesModule::ExportCoordinatesModule() : Module()
{
    // Initialise Module - set up keywords etc.
    initialise();
}

ExportCoordinatesModule::~ExportCoordinatesModule() {}

/*
 * Instances
 */

// Create instance of this module
Module *ExportCoordinatesModule::createInstance() const { return new ExportCoordinatesModule; }
