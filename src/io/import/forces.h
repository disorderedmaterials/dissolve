/*
    *** Import - Forces
    *** src/io/import/forces.h
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
#include "templates/array.h"

// Forward Declarations
class ProcessPool;

// Forces Import Formats
class ForceImportFileFormat : public FileAndFormat
{
    public:
    // Forces Formats
    enum ForceImportFormat
    {
        DLPOLYForces,
        MoscitoForces,
        SimpleForces,
        nForceImportFormats
    };
    ForceImportFileFormat(ForceImportFormat format = SimpleForces);
    ForceImportFileFormat(std::string_view filename, ForceImportFormat format = SimpleForces);
    ~ForceImportFileFormat();

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
    // Return enum options for ForceImportFormat
    static EnumOptions<ForceImportFileFormat::ForceImportFormat> &forceImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as ForceImportFormat
    ForceImportFormat forceFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return true; }

    /*
     * Import Functions
     */
    private:
    // Import DL_POLY forces through specified parser
    bool importDLPOLY(LineParser &parser, Array<double> &fx, Array<double> &fy, Array<double> &fz);
    // Import Moscito forces through specified parser
    bool importMoscito(LineParser &parser, Array<double> &fx, Array<double> &fy, Array<double> &fz);
    // Import simple formatted forces through specified parser
    bool importSimple(LineParser &parser, Array<double> &fx, Array<double> &fy, Array<double> &fz);

    public:
    // Import forces using current filename and format
    bool importData(Array<double> &fx, Array<double> &fy, Array<double> &fz, ProcessPool *procPool = nullptr);
    // Import forces using supplied parser and current format
    bool importData(LineParser &parser, Array<double> &fx, Array<double> &fy, Array<double> &fz);
};
