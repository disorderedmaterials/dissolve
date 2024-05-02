// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class Species;

// Species Export Formats
class SpeciesExportFileFormat : public FileAndFormat
{
    public:
    // Available coordinate formats
    enum class SpeciesExportFormat
    {
        XYZ,
        DLPOLY
    };
    SpeciesExportFileFormat(std::string_view filename = "", SpeciesExportFormat format = SpeciesExportFormat::XYZ);
    ~SpeciesExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<SpeciesExportFileFormat::SpeciesExportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return false; }

    /*
     * Export Functions
     */
    private:
    // Export species as XYZ
    bool exportXYZ(LineParser &parser, const Species *sp);

    public:
    // Export species using current filename and format
    bool exportData(const Species *sp);
};
