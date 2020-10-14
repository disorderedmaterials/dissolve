// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/import/import.h"

// Return type of module
std::string_view ImportModule::type() const { return "Import"; }

// Return category for module
std::string_view ImportModule::category() const { return "Input / Output"; }

// Return brief description of module
std::string_view ImportModule::brief() const { return "Import data to the target Configuration"; }

// Return the number of Configuration targets this Module requires
int ImportModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
