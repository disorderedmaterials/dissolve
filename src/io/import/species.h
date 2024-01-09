// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class ProcessPool;

// Species Import Formats
class SpeciesImportFileFormat : public FileAndFormat
{
    public:
    // Species Import Formats
    enum class SpeciesImportFormat
    {
        XYZ
    };
    explicit SpeciesImportFileFormat(std::string_view filename = "", SpeciesImportFormat format = SpeciesImportFormat::XYZ);
    ~SpeciesImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    private:
    // Set up keywords for the format
    void setUpKeywords();

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<SpeciesImportFileFormat::SpeciesImportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

    /*
     * Import Functions
     */
    private:
    // Import xyz coordinates through specified parser
    bool importXYZ(LineParser &parser, Species *sp);

    public:
    // Import coordinates direct to configuration using current filename and format
    bool importData(Species *sp, const ProcessPool *procPool = nullptr);
    // Import coordinates using supplied parser and current format
    bool importData(LineParser &parser, std::vector<Vec3<double>> &r);
    // Import coordinates direct to configuration using supplied parser and current format
    bool importData(LineParser &parser, Species *sp);
};
