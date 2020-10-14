// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/sanitycheck/sanitycheck.h"

// Return type of module
std::string_view SanityCheckModule::type() const { return "SanityCheck"; }

// Return category for module
std::string_view SanityCheckModule::category() const { return "Checks & Tests"; }

// Return brief description of module
std::string_view SanityCheckModule::brief() const { return "Perform data sanity checks across all processes"; }

// Return the number of Configuration targets this Module requires
int SanityCheckModule::nRequiredTargets() const { return Module::ZeroTargets; }
