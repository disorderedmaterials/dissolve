// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/export_coordinates/exportcoords.h"

// Return type of module
std::string_view ExportCoordinatesModule::type() const { return "ExportCoordinates"; }

// Return category for module
std::string_view ExportCoordinatesModule::category() const { return "Export"; }

// Return brief description of module
std::string_view ExportCoordinatesModule::brief() const { return "Export coordinates for one or more Configurations"; }

// Return the number of Configuration targets this Module requires
int ExportCoordinatesModule::nRequiredTargets() const { return Module::ExactlyOneTarget; }
