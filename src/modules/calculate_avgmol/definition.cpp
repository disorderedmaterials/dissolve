// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_avgmol/avgmol.h"

// Return type of module
std::string_view CalculateAvgMolModule::type() const { return "CalculateAvgMol"; }

// Return category for module
std::string_view CalculateAvgMolModule::category() const { return "Analysis / Geometry"; }

// Return brief description of module
std::string_view CalculateAvgMolModule::brief() const
{
    return "Calculate average atomic positions of a species around an oriented site";
}

// Return the number of Configuration targets this Module requires
int CalculateAvgMolModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
