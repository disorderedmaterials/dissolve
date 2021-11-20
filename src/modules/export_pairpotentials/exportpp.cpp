// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/export_pairpotentials/exportpp.h"
#include "keywords/fileandformat.h"

ExportPairPotentialsModule::ExportPairPotentialsModule() : Module("ExportPairPotentials")
{
    // Format
    keywords_.add<FileAndFormatKeyword>("Format", "Format", "Basename and format in which to write potentials",
                                        pairPotentialFormat_, "EndFormat");
}
