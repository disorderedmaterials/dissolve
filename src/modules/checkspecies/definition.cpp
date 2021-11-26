// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/checkspecies/checkspecies.h"

// Return type of module
std::string_view CheckSpeciesModule::type() const { return "CheckSpecies"; }

// Return category for module
std::string_view CheckSpeciesModule::category() const { return "Checks & Tests"; }

// Return brief description of module
std::string_view CheckSpeciesModule::brief() const { return "Check the contents of a Species against reference values"; }
