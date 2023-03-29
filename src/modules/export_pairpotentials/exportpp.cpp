// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/export_pairpotentials/exportpp.h"
#include "keywords/fileandformat.h"

ExportPairPotentialsModule::ExportPairPotentialsModule() : Module(ModuleTypes::ExportPairPotentials)
{
    keywords_.setOrganisation("File");
    keywords_.add<FileAndFormatKeyword>("Format", "Basename and format in which to write potentials", pairPotentialFormat_,
                                        "EndFormat");
}
