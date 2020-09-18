/*
    *** Import - Data2D
    *** src/io/import/data2d.h
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

#include "base/enumoptions.h"
#include "io/fileandformat.h"
#include "templates/vector3.h"

// Forward Declarations
class Data2D;
class ProcessPool;
class Vec3DoubleKeyword;

// Data2D Import Formats
class Data2DImportFileFormat : public FileAndFormat
{
    public:
    // Available Data2D formats
    enum Data2DImportFormat
    {
        CartesianData2D,
        nData2DImportFormats
    };
    Data2DImportFileFormat(Data2DImportFormat format = CartesianData2D);
    Data2DImportFileFormat(std::string_view filename, Data2DImportFormat format = CartesianData2D);
    ~Data2DImportFileFormat();

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
    // Return enum options for Data2DImportFormat
    static EnumOptions<Data2DImportFileFormat::Data2DImportFormat> &data2DImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as Data2DImportFormat
    Data2DImportFormat data2DFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return true; }

    /*
     * Data Import
     */
    private:
    // Import cartesian data from supplied parser
    bool importCartesian(LineParser &parser, Data2D &data);

    public:
    // Import Data2D using current filename and format
    bool importData(Data2D &data, ProcessPool *procPool = nullptr);
    // Import Data2D using supplied parser and current format
    bool importData(LineParser &parser, Data2D &data);
};
