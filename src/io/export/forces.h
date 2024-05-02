// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class Configuration;

// Force Export Formats
class ForceExportFileFormat : public FileAndFormat
{
    public:
    // Available force formats
    enum class ForceExportFormat
    {
        Simple
    };
    ForceExportFileFormat(std::string_view filename = "", ForceExportFormat format = ForceExportFormat::Simple);
    ~ForceExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<ForceExportFileFormat::ForceExportFormat> formats_;

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
    // Export supplied forces in simple format
    bool exportSimple(LineParser &parser, const std::vector<Vec3<double>> &f);

    public:
    // Export supplied forces using current filename and format
    bool exportData(const std::vector<Vec3<double>> &f);
};
