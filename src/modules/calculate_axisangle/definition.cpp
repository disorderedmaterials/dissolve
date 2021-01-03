// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_axisangle/axisangle.h"

// Return type of module
std::string_view CalculateAxisAngleModule::type() const { return "CalculateAxisAngle"; }

// Return category for module
std::string_view CalculateAxisAngleModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view CalculateAxisAngleModule::brief() const { return "Calculate distance/angle map between site axes"; }

// Return the number of Configuration targets this Module requires
int CalculateAxisAngleModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
