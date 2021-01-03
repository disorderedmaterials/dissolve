// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_dangle/dangle.h"

// Return type of module
std::string_view CalculateDAngleModule::type() const { return "CalculateDAngle"; }

// Return category for module
std::string_view CalculateDAngleModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view CalculateDAngleModule::brief() const { return "Calculate distance/angle maps"; }

// Return the number of Configuration targets this Module requires
int CalculateDAngleModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
