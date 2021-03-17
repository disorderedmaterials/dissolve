// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/import/import.h"

// Perform any necessary initialisation for the Module
void ImportModule::initialise()
{
    // Trajectory Import
    keywords_.add("Trajectory", new BoolKeyword(true), "ReadTrajectory", "Read sequential frames from a trajectory file",
                  "<True|False>");
    keywords_.add("Trajectory", new FileAndFormatKeyword(trajectoryFile_, "EndTrajectoryFile"), "TrajectoryFile",
                  "Source trajectory file for frames (if ReadTrajectory == True)");
}
