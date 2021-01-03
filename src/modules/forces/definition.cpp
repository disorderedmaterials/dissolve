// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/forces/forces.h"

// Return type of module
std::string_view ForcesModule::type() const { return "Forces"; }

// Return category for module
std::string_view ForcesModule::category() const { return "Forcefield"; }

// Return brief description of module
std::string_view ForcesModule::brief() const { return "Calculate the total atomic forces within a Configuration"; }

// Return the number of Configuration targets this Module requires
int ForcesModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
