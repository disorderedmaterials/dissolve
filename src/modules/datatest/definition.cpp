// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/datatest/datatest.h"

// Return type of module
std::string_view DataTestModule::type() const { return "DataTest"; }

// Return category for module
std::string_view DataTestModule::category() const { return "Checks & Tests"; }

// Return brief description of module
std::string_view DataTestModule::brief() const { return "Test named data in other modules against reference data"; }

// Return the number of Configuration targets this Module requires
int DataTestModule::nRequiredTargets() const { return Module::ZeroTargets; }
