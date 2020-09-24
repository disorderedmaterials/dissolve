// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/refine/refine.h"

// Return type of module
std::string_view RefineModule::type() const { return "Refine"; }

// Return category for module
std::string_view RefineModule::category() const { return "Forcefield"; }

// Return brief description of module
std::string_view RefineModule::brief() const
{
    return "Refine interatomic potentials based on differences between calculated and reference data";
}

// Return the number of Configuration targets this Module requires
int RefineModule::nRequiredTargets() const { return Module::ZeroTargets; }
