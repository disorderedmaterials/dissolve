// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/export/species.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "data/atomicMasses.h"

SpeciesExportFileFormat::SpeciesExportFileFormat(std::string_view filename, SpeciesExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<SpeciesExportFileFormat::SpeciesExportFormat>(
        "SpeciesExportFileFormat", {{SpeciesExportFormat::XYZ, "xyz", "Simple XYZ Speciess"},
                                    {SpeciesExportFormat::DLPOLY, "dlpoly", "DL_POLY CONFIG File"}});
}

/*
 * Export Functions
 */

// Export coordinates as XYZ
bool SpeciesExportFileFormat::exportXYZ(LineParser &parser, const Species *sp)
{
    // Export number of atoms and title
    if (!parser.writeLineF("{}\n", sp->nAtoms()))
        return false;
    if (!parser.writeLineF("{}\n", sp->name()))
        return false;

    // Export Atoms
    for (const auto &i : sp->atoms())
        if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.Z()), i.r().x, i.r().y, i.r().z))
            return false;

    return true;
}

// Export coordinates using current filename and format
bool SpeciesExportFileFormat::exportData(const Species *sp)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for SpeciesExportFileFormat so can't export.\n");

    // Open the file
    LineParser parser;
    if (!parser.openOutput(filename_))
    {
        parser.closeFiles();
        return false;
    }

    // Write data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (SpeciesExportFormat::XYZ):
            result = exportXYZ(parser, sp);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Species format '{}' export has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}
