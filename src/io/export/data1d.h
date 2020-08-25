/*
    *** Export - Data1D
    *** src/io/export/data1d.h
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
class Data1D;

// Data1D Export Formats
class Data1DExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum Data1DExportFormat
    {
        XYData1D,
        nData1DExportFormats
    };
    Data1DExportFileFormat(std::string_view filename = "", Data1DExportFormat format = Data1DExportFileFormat::XYData1D);

    /*
     * Format Access
     */
    private:
    // Return enum options for Data1DExportFormat
    static EnumOptions<Data1DExportFileFormat::Data1DExportFormat> &data1DExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as Data1DExportFormat
    Data1DExportFormat data1DFormat() const;

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
    // Export Data1D as simple XY (or XYE) data
    bool exportXY(LineParser &parser, const Data1D &data);

    public:
    // Export Data1D using current filename and format
    bool exportData(const Data1D &data);
};
