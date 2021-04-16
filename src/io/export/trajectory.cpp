// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/export/trajectory.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "data/elements.h"

TrajectoryExportFileFormat::TrajectoryExportFileFormat(std::string_view filename, TrajectoryExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for TrajectoryExportFormat
EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat> TrajectoryExportFileFormat::trajectoryExportFormats()
{
    return EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat>(
        "TrajectoryExportFileFormat", {{TrajectoryExportFileFormat::XYZTrajectory, "xyz", "XYZ Trajectory"}});
}

// Return number of available formats
int TrajectoryExportFileFormat::nFormats() const { return TrajectoryExportFileFormat::nTrajectoryExportFormats; }

// Return format keyword for supplied index
std::string TrajectoryExportFileFormat::formatKeyword(int id) const { return trajectoryExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string TrajectoryExportFileFormat::formatDescription(int id) const
{
    return trajectoryExportFormats().descriptionByIndex(id);
}

// Return current format as TrajectoryExportFormat
TrajectoryExportFileFormat::TrajectoryExportFormat TrajectoryExportFileFormat::trajectoryFormat() const
{
    return (TrajectoryExportFileFormat::TrajectoryExportFormat)format_;
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
    for (auto i : cfg->atoms())
        if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i->speciesAtom()->Z()), i->r().x,
                               i->r().y, i->r().z))
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

        if (format_ == XYZTrajectory)
            headerResult = true;
        else
            headerResult = Messenger::error("Unrecognised trajectory format so can't write header.\nKnown formats are:\n");
        printAvailableFormats();

        if (!headerResult)
            return false;
    }

    // Append frame in supplied format
    auto frameResult = false;
    if (trajectoryFormat() == TrajectoryExportFileFormat::XYZTrajectory)
        frameResult = exportXYZ(parser, cfg);
    else
    {
        Messenger::error("Unrecognised trajectory format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    parser.closeFiles();

    return frameResult;
}
