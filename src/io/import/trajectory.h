/*
    *** Import - Trajectory
    *** src/io/import/trajectory.h
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

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
/* none */

// Trajectory Import Formats
class TrajectoryImportFileFormat : public FileAndFormat
{
    public:
    // Available trajectory formats
    enum TrajectoryImportFormat
    {
        XYZTrajectory,
        nTrajectoryImportFormats
    };
    TrajectoryImportFileFormat(TrajectoryImportFormat format = XYZTrajectory);
    TrajectoryImportFileFormat(std::string_view filename, TrajectoryImportFormat format = XYZTrajectory);
    ~TrajectoryImportFileFormat();

    /*
     * Keyword Options
     */
    private:
    // Set up keywords for the format
    void setUpKeywords();

    /*
     * Format Access
     */
    private:
    // Return enum options for TrajectoryImportFileFormat
    static EnumOptions<TrajectoryImportFileFormat::TrajectoryImportFormat> &trajectoryImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as TrajectoryImportFormat
    TrajectoryImportFormat trajectoryFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return true; }
};
