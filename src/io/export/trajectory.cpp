// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/export/trajectory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "data/elements.h"

TrajectoryExportFileFormat::TrajectoryExportFileFormat(std::string_view filename, TrajectoryExportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat>(
        "TrajectoryExportFileFormat", {{TrajectoryExportFormat::XYZ, "xyz", "XYZ Trajectory"}}, format);
}

/*
 * Export Functions
 */

// Append XYZ frame to trajectory
bool TrajectoryExportFileFormat::exportXYZ(LineParser &parser, Configuration *cfg)
{
    // Write number of atoms and title
    if (!parser.writeLineF("{}\n", cfg->nAtoms()))
        return false;
    if (!parser.writeLineF("{} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;

    // Write Atoms
    for (const auto &i : cfg->atoms())
        if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.speciesAtom()->Z()), i.r().x,
                               i.r().y, i.r().z))
            return false;

    return true;
}

// Append trajectory using current filename and format
bool TrajectoryExportFileFormat::exportData(Configuration *cfg)
{
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

        if (formats_.enumeration() == TrajectoryExportFormat::XYZ)
            headerResult = true;
        else
            headerResult = Messenger::error("Unrecognised trajectory format so can't write header.\nKnown formats are:\n");
        printAvailableFormats();

        if (!headerResult)
            return false;
    }

    // Append frame in supplied format
    auto frameResult = false;
    switch (formats_.enumeration())
    {
        case (TrajectoryExportFormat::XYZ):
            frameResult = exportXYZ(parser, cfg);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Trajectory format '{}' export has not been implemented.\n", formats_.keyword())));
    }

    parser.closeFiles();

    return frameResult;
}
