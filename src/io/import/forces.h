// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class ProcessPool;

// Forces Import Formats
class ForceImportFileFormat : public FileAndFormat
{
    public:
    // Forces Formats
    enum class ForceImportFormat
    {
        DLPOLY,
        Moscito,
        Simple
    };
    ForceImportFileFormat(std::string_view filename = "", ForceImportFormat format = ForceImportFormat::Simple);
    ~ForceImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    private:
    // Factor by which to multiply incoming forces
    double factor_{1.0};

    private:
    // Set up keywords for the format
    void setUpKeywords();

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<ForceImportFileFormat::ForceImportFormat> formats_;

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
    // Import DL_POLY forces through specified parser
    bool importDLPOLY(LineParser &parser, std::vector<Vec3<double>> &f);
    // Import Moscito forces through specified parser
    bool importMoscito(LineParser &parser, std::vector<Vec3<double>> &f);
    // Import simple formatted forces through specified parser
    bool importSimple(LineParser &parser, std::vector<Vec3<double>> &f);

    public:
    // Import forces using current filename and format
    bool importData(std::vector<Vec3<double>> &f, const ProcessPool *procPool = nullptr);
    // Import forces using supplied parser and current format
    bool importData(LineParser &parser, std::vector<Vec3<double>> &f);
};
