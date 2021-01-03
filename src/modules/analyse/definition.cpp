// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/analyse/analyse.h"

// Return type of module
std::string_view AnalyseModule::type() const { return "Analyse"; }

// Return category for module
std::string_view AnalyseModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view AnalyseModule::brief() const { return "Perform custom analysis of one or more Configurations"; }

// Return the number of Configuration targets this Module requires
int AnalyseModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
