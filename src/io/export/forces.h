// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Configuration;

// Force Export Formats
class ForceExportFileFormat : public FileAndFormat
{
    public:
    // Available force formats
    enum ForceExportFormat
    {
        SimpleForces,
        nForceExportFormats
    };
    ForceExportFileFormat(std::string_view filename = "", ForceExportFormat format = SimpleForces);

    /*
     * Format Access
     */
    public:
    // Return enum options for ForceExportFormat
    static EnumOptions<ForceExportFileFormat::ForceExportFormat> forceExportFormats();
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as ForceExportFormat
    ForceExportFormat forceFormat() const;

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
    // Export supplied forces in simple format
    bool exportSimple(LineParser &parser, const std::vector<Vec3<double>> &f);

    public:
    // Export supplied forces using current filename and format
    bool exportData(const std::vector<Vec3<double>> &f);
};
