/*
    *** Export - Forces
    *** src/io/export/forces.h
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
class Configuration;

// Force Export Formats
class ForceExportFileFormat : public FileAndFormat
{
    public:
    // Available force formats
    enum ForceExportFormat
    {
        SimpleForces,
        nForceExportFormats
    };
    ForceExportFileFormat(std::string_view filename = "", ForceExportFormat format = SimpleForces);

    /*
     * Format Access
     */
    public:
    // Return enum options for ForceExportFormat
    static EnumOptions<ForceExportFileFormat::ForceExportFormat> forceExportFormats();
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as ForceExportFormat
    ForceExportFormat forceFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return false; }

    /*
     * Export Functions
     */
    private:
    // Export supplied forces in simple format
    bool exportSimple(LineParser &parser, const Array<double> &fx, const Array<double> &fy, const Array<double> &fz);

    public:
    // Export supplied forces using current filename and format
    bool exportData(const Array<double> &fx, const Array<double> &fy, const Array<double> &fz);
};
