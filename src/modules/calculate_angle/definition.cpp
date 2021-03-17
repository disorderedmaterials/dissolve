// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_angle/angle.h"

// Return type of module
std::string_view CalculateAngleModule::type() const { return "CalculateAngle"; }

// Return category for module
std::string_view CalculateAngleModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view CalculateAngleModule::brief() const { return "Calculate distance/angle maps"; }

// Return the number of Configuration targets this Module requires
int CalculateAngleModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
