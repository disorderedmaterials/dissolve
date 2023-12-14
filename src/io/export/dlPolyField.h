// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class Configuration;

// Coordinate Export Formats
class DlPolyFieldExportFileFormat : public FileAndFormat
{
    public:
    // Available DlPolyField formats
    enum class DlPolyFieldExportFormat
    {
        DLPOLY
    };
    DlPolyFieldExportFileFormat(std::string_view filename = "", DlPolyFieldExportFormat format = DlPolyFieldExportFormat::DLPOLY);
    ~DlPolyFieldExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<DlPolyFieldExportFileFormat::DlPolyFieldExportFormat> formats_;

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
    // Export Configuration as DL_POLY FIELD
    bool exportDLPOLY(LineParser &parser, Configuration *cfg);

    public:
    // Export Configuration using current filename and format
    bool exportData(Configuration *cfg);

};


