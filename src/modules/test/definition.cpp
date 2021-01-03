// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/test/test.h"

// Return type of module
std::string_view TestModule::type() const { return "Test"; }

// Return category for module
std::string_view TestModule::category() const { return "HIDDEN"; }

// Return brief description of module
std::string_view TestModule::brief() const { return "Development Module"; }

// Return the number of Configuration targets this Module requires
int TestModule::nRequiredTargets() const { return Module::ZeroTargets; }
