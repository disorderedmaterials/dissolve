// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/molshake/molshake.h"

// Return type of module
std::string_view MolShakeModule::type() const { return "MolShake"; }

// Return category for module
std::string_view MolShakeModule::category() const { return "Evolution"; }

// Return brief description of module
std::string_view MolShakeModule::brief() const { return "Perform molecular Monte Carlo moves"; }

// Return the number of Configuration targets this Module requires
int MolShakeModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
