// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/bragg/bragg.h"

// Return type of module
std::string_view BraggModule::type() const { return "Bragg"; }

// Return category for module
std::string_view BraggModule::category() const { return "Correlation Functions"; }

// Return brief description of module
std::string_view BraggModule::brief() const { return "Calculate Bragg scattering"; }

// Return the number of Configuration targets this Module requires
int BraggModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
