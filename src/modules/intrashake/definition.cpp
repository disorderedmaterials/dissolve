// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/intrashake/intrashake.h"

// Return type of module
std::string_view IntraShakeModule::type() const { return "IntraShake"; }

// Return category for module
std::string_view IntraShakeModule::category() const { return "Evolution"; }

// Return brief description of module
std::string_view IntraShakeModule::brief() const
{
    return "Perform Monte Carlo shakes on intramolecular terms within Molecules";
}

// Return the number of Configuration targets this Module requires
int IntraShakeModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
