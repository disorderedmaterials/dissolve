// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/md/md.h"

// Return type of module
std::string_view MDModule::type() const { return "MD"; }

// Return category for module
std::string_view MDModule::category() const { return "Evolution"; }

// Return brief description of module
std::string_view MDModule::brief() const { return "Evolve a Configuration using molecular dynamics"; }

// Return the number of Configuration targets this Module requires
int MDModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
