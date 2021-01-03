// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/geomopt/geomopt.h"

// Return type of module
std::string_view GeometryOptimisationModule::type() const { return "GeometryOptimisation"; }

// Return category for module
std::string_view GeometryOptimisationModule::category() const { return "Optimisation"; }

// Return brief description of module
std::string_view GeometryOptimisationModule::brief() const { return "Optimise geometry with respect to energy (minimise)"; }

// Return the number of Configuration targets this Module requires
int GeometryOptimisationModule::nRequiredTargets() const { return Module::OneOrMoreTargets; }
