/*
    *** Import - Trajectory
    *** src/io/import/trajectory.cpp
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

#include "io/import/trajectory.h"
#include "base/sysfunc.h"

TrajectoryImportFileFormat::TrajectoryImportFileFormat(TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(format)
{
    setUpKeywords();
}
TrajectoryImportFileFormat::TrajectoryImportFileFormat(std::string_view filename,
                                                       TrajectoryImportFileFormat::TrajectoryImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

TrajectoryImportFileFormat::~TrajectoryImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void TrajectoryImportFileFormat::setUpKeywords() {}

/*
 * Format Access
 */

// Return enum options for TrajectoryImportFormat
EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> &TrajectoryImportFileFormat::trajectoryImportFormats()
{
    static EnumOptionsList TrajectoryImportFormats =
        EnumOptionsList() << EnumOption(TrajectoryImportFileFormat::XYZTrajectory, "xyz", "XYZ Trajectory");

    static EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> options("TrajectoryImportFileFormat",
                                                                                   TrajectoryImportFormats);

    return options;
}

// Return number of available formats
int TrajectoryImportFileFormat::nFormats() const { return TrajectoryImportFileFormat::nTrajectoryImportFormats; }

// Return format keyword for supplied index
std::string_view TrajectoryImportFileFormat::formatKeyword(int id) const
{
    return trajectoryImportFormats().keywordByIndex(id);
}

// Return description string for supplied index
std::string_view TrajectoryImportFileFormat::formatDescription(int id) const
{
    return trajectoryImportFormats().descriptionByIndex(id);
}

// Return current format as TrajectoryImportFormat
TrajectoryImportFileFormat::TrajectoryImportFormat TrajectoryImportFileFormat::trajectoryFormat() const
{
    return (TrajectoryImportFileFormat::TrajectoryImportFormat)format_;
}
