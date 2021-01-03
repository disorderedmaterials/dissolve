// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/rdf/rdf.h"

// Return type of module
std::string_view RDFModule::type() const { return "RDF"; }

// Return category for module
std::string_view RDFModule::category() const { return "Correlation Functions"; }

// Return brief description of module
std::string_view RDFModule::brief() const { return "Calculate partial and total g(r)"; }

// Return the number of Configuration targets this Module requires
int RDFModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
