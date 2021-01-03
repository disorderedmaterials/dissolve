// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_sdf/sdf.h"

// Return type of module
std::string_view CalculateSDFModule::type() const { return "CalculateSDF"; }

// Return category for module
std::string_view CalculateSDFModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view CalculateSDFModule::brief() const { return "Calculate spatial density functions around oriented sites"; }

// Return the number of Configuration targets this Module requires
int CalculateSDFModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
