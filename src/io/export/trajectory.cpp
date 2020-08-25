/*
    *** Export - Trajectory
    *** src/io/export/trajectory.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat> &TrajectoryExportFileFormat::trajectoryExportFormats()
{
    static EnumOptionsList TrajectoryExportFormats =
        EnumOptionsList() << EnumOption(TrajectoryExportFileFormat::XYZTrajectory, "xyz", "XYZ Trajectory");

    static EnumOptions<TrajectoryExportFileFormat::TrajectoryExportFormat> options("TrajectoryExportFileFormat",
                                                                                   TrajectoryExportFormats);

    return options;
}

// Return number of available formats
int TrajectoryExportFileFormat::nFormats() const { return TrajectoryExportFileFormat::nTrajectoryExportFormats; }

// Return format keyword for supplied index
std::string_view TrajectoryExportFileFormat::formatKeyword(int id) const
{
    return trajectoryExportFormats().keywordByIndex(id);
}

// Return description string for supplied index
std::string_view TrajectoryExportFileFormat::formatDescription(int id) const
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
    for (int n = 0; n < cfg->nAtoms(); ++n)
    {
        Atom *i = cfg->atom(n);
        if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", i->speciesAtom()->element()->symbol(), i->r().x,
                               i->r().y, i->r().z))
            return false;
    }

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
        // 		else if (format_ == OneThatNeedsAHeaderTrajectory) headerResult = writeAHeader(parser, cfg);
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
