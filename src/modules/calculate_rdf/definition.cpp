// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/calculate_rdf/rdf.h"

// Return type of module
std::string_view CalculateRDFModule::type() const { return "CalculateRDF"; }

// Return category for module
std::string_view CalculateRDFModule::category() const { return "Analysis"; }

// Return brief description of module
std::string_view CalculateRDFModule::brief() const { return "Calculate radial distribution functions between sites"; }

// Return the number of Configuration targets this Module requires
int CalculateRDFModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
