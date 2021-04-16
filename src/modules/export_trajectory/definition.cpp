// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/export_trajectory/exporttraj.h"

// Return type of module
std::string_view ExportTrajectoryModule::type() const { return "ExportTrajectory"; }

// Return category for module
std::string_view ExportTrajectoryModule::category() const { return "Export"; }

// Return brief description of module
std::string_view ExportTrajectoryModule::brief() const { return "Export trajectory for a Configuration"; }

// Return the number of Configuration targets this Module requires
int ExportTrajectoryModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
