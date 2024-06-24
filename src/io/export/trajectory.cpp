// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/export/trajectory.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
#include "data/elements.h"

TrajectoryExportFileFormat::TrajectoryExportFileFormat(std::string_view filename, TrajectoryExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat>(
        "TrajectoryExportFileFormat", {{TrajectoryExportFormat::XYZ, "xyz", "XYZ Trajectory"},
                                       {TrajectoryExportFormat::XYZExtended, "xyzExt", "XYZ Trajectory Extended"}});
}

/*
 * Export Functions
 */

// Append XYZ frame to trajectory
bool TrajectoryExportFileFormat::exportXYZ(LineParser &parser, Configuration *cfg, bool extended)
{
    // Write number of atoms and title
    if (!parser.writeLineF("{}\n", cfg->nAtoms()))
        return false;
    if (!parser.writeLineF("{} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;

    // Write Atoms
    for (const auto &i : cfg->atoms())
    {
        if (extended)
        {
            if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}  {:<6d}  {}\n", Elements::symbol(i.speciesAtom()->Z()),
                                   i.r().x, i.r().y, i.r().z, i.speciesAtom()->index() + 1,
                                   i.speciesAtom()->atomType()->name()))
                return false;
        }
        else
        {
            if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.speciesAtom()->Z()), i.r().x,
                                   i.r().y, i.r().z))
                return false;
        }
    }

    return true;
}

// Append trajectory using current filename and format
bool TrajectoryExportFileFormat::exportData(Configuration *cfg)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for TrajectoryExportFileFormat so can't export.\n");

    // Make an initial check to see if the specified file exists
    auto fileExists = DissolveSys::fileExists(filename_);

    // Open the specified file for appending
    LineParser parser;
    if (!parser.appendOutput(filename_))
    {
        parser.closeFiles();
        return false;
    }

    // Write header?
    if (!fileExists)
    {
        auto headerResult = false;

        if (*formatIndex_ >= 0 && *formatIndex_ < formats_.nOptions())
            headerResult = true;
        else
            headerResult = Messenger::error("Unrecognised trajectory format so can't write header.\nKnown formats are:\n");
        printAvailableFormats();

        if (!headerResult)
            return false;
    }

    // Append frame in supplied format
    auto frameResult = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (TrajectoryExportFormat::XYZ):
            frameResult = exportXYZ(parser, cfg, false);
            break;
        case (TrajectoryExportFormat::XYZExtended):
            frameResult = exportXYZ(parser, cfg, true);
            break;
        default:
            throw(std::runtime_error(fmt::format("Trajectory format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    parser.closeFiles();

    return frameResult;
}
