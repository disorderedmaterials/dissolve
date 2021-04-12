// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/import_trajectory/importtraj.h"

ImportTrajectoryModule::ImportTrajectoryModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *ImportTrajectoryModule::createInstance() const { return new ImportTrajectoryModule; }
