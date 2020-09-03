/*
    *** Export - PairPotential
    *** src/io/export/pairpotential.h
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
class PairPotential;

// PairPotential Export Formats
class PairPotentialExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum PairPotentialExportFormat
    {
        BlockPairPotential,
        DLPOLYTABLEPairPotential,
        nPairPotentialExportFormats
    };
    PairPotentialExportFileFormat(std::string_view filename = "", PairPotentialExportFormat format = BlockPairPotential);

    /*
     * Format Access
     */
    private:
    // Return enum options for PairPotentialExportFormat
    static EnumOptions<PairPotentialExportFileFormat::PairPotentialExportFormat> &pairPotentialExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string_view formatKeyword(int id) const;
    // Return description string for supplied index
    std::string_view formatDescription(int id) const;
    // Return current format as PairPotentialExportFormat
    PairPotentialExportFormat pairPotentialFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return false; }

    /*
     * Static Functions - PairPotentials
     */
    private:
    // Export PairPotential as simple block data
    bool exportBlock(LineParser &parser, PairPotential *pp);
    // Export PairPotential as a DL_POLY TABLE file
    bool exportDLPOLY(LineParser &parser, PairPotential *pp);

    public:
    // Export PairPotential using current filename and format
    bool exportData(PairPotential *pp);
};
