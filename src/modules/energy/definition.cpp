// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/energy/energy.h"

// Return type of module
std::string_view EnergyModule::type() const { return "Energy"; }

// Return category for module
std::string_view EnergyModule::category() const { return "Forcefield"; }

// Return brief description of module
std::string_view EnergyModule::brief() const { return "Calculate the total energy of a Configuration"; }

// Return the number of Configuration targets this Module requires
int EnergyModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
