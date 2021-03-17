// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_cn/cn.h"

// Return type of module
std::string_view CalculateCNModule::type() const { return "CalculateCN"; }

// Return category for module
std::string_view CalculateCNModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view CalculateCNModule::brief() const
{
    return "Calculate coordination numbers from an existing radial distribution function";
}

// Return the number of Configuration targets this Module requires
int CalculateCNModule::nRequiredTargets() const { return Module::ZeroTargets; }
