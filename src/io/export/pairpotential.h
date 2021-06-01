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
    ~PairPotentialExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<PairPotentialExportFileFormat::PairPotentialExportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return false; }

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
