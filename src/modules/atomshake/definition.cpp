// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/atomshake/atomshake.h"

// Return type of module
std::string_view AtomShakeModule::type() const { return "AtomShake"; }

// Return category for module
std::string_view AtomShakeModule::category() const { return "Evolution"; }

// Return brief description of module
std::string_view AtomShakeModule::brief() const { return "Perform atomic Monte Carlo on all atoms"; }

// Return the number of Configuration targets this Module requires
int AtomShakeModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
