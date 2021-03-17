// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/export_trajectory/exporttraj.h"

// Perform any necessary initialisation for the Module
void ExportTrajectoryModule::initialise()
{
    // Format
    keywords_.add("Format", new FileAndFormatKeyword(trajectoryFormat_, "EndFormat"), "Format", "File / format for trajectory");
}
