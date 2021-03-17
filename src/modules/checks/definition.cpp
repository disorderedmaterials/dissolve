// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/checks/checks.h"

// Return type of module
std::string_view ChecksModule::type() const { return "Checks"; }

// Return category for module
std::string_view ChecksModule::category() const { return "Checks & Tests"; }

// Return brief description of module
std::string_view ChecksModule::brief() const { return "Conduct simple checks on Configurations"; }

// Return the number of Configuration targets this Module requires
int ChecksModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
