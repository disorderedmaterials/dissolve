// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    static EnumOptions<PairPotentialExportFileFormat::PairPotentialExportFormat> pairPotentialExportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
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
