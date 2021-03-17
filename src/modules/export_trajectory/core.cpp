// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/export_trajectory/exporttraj.h"

ExportTrajectoryModule::ExportTrajectoryModule() : Module(nRequiredTargets())
{
    // Initialise Module - set up keywords etc.
    initialise();
}

/*
 * Instances
 */

// Create instance of this module
Module *ExportTrajectoryModule::createInstance() const { return new ExportTrajectoryModule; }
