// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "modules/export_pairpotentials/exportpp.h"

// Return type of module
std::string_view ExportPairPotentialsModule::type() const { return "ExportPairPotentials"; }

// Return category for module
std::string_view ExportPairPotentialsModule::category() const { return "Input / Output"; }

// Return brief description of module
std::string_view ExportPairPotentialsModule::brief() const { return "Export pair potentials"; }

// Return the number of Configuration targets this Module requires
int ExportPairPotentialsModule::nRequiredTargets() const { return Module::ZeroTargets; }
