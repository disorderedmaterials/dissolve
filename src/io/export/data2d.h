/*
    *** Export - Data2D
    *** src/io/export/data2d.h
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
class Data2D;

// Data2D Export Formats
class Data2DExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum Data2DExportFormat
    {
        BlockData2D,
        CartesianData2D,
        nData2DExportFormats
    };
    Data2DExportFileFormat(std::string_view filename = "", Data2DExportFormat format = Data2DExportFileFormat::BlockData2D);

    /*
     * Format Access
     */
    private:
    // Return enum options for Data2DExportFormat
    static EnumOptions<Data2DExportFileFormat::Data2DExportFormat> &data2DExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as Data2DExportFormat
    Data2DExportFormat data2DFormat() const;

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
    // Export Data2D as simple block data
    bool exportBlock(LineParser &parser, const Data2D &data);
    // Export Data2D as cartesian data
    bool exportCartesian(LineParser &parser, const Data2D &data);

    public:
    // Export Data2D using current filename and format
    bool exportData(const Data2D &data);
};
