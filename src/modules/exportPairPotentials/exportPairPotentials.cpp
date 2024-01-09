// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/exportPairPotentials/exportPairPotentials.h"
#include "keywords/fileAndFormat.h"

ExportPairPotentialsModule::ExportPairPotentialsModule() : Module(ModuleTypes::ExportPairPotentials)
{
    keywords_.setOrganisation("Options", "File");
    keywords_.add<FileAndFormatKeyword>("Format", "Basename and format in which to write potentials", pairPotentialFormat_,
                                        "EndFormat");
}
