// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/export_pairpotentials/exportpp.h"

// Perform any necessary initialisation for the Module
void ExportPairPotentialsModule::initialise()
{
    // Format
    keywords_.add<FileAndFormatKeyword>("Format", "Format", "Basename and format in which to write potentials",
                                        pairPotentialFormat_, "EndFormat");
}
