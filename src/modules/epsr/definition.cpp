// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/epsr/epsr.h"

// Return type of module
std::string_view EPSRModule::type() const { return "EPSR"; }

// Return category for module
std::string_view EPSRModule::category() const { return "Forcefield"; }

// Return brief description of module
std::string_view EPSRModule::brief() const { return "Refine interatomic potentials in a manner consistent with EPSR"; }

// Return the number of Configuration targets this Module requires
int EPSRModule::nRequiredTargets() const { return Module::ZeroTargets; }
