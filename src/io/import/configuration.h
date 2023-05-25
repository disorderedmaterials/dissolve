// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class ProcessPool;

// Configuration Import Formats
class ConfigurationImportFileFormat : public FileAndFormat
{
    public:
    // Configuration Import Formats
    enum class ConfigurationImportFormat
    {
        DLPOLY,
        EPSR,
        Moscito,
        XYZ
    };
    explicit ConfigurationImportFileFormat(std::string_view filename = "",
                                           ConfigurationImportFormat format = ConfigurationImportFormat::XYZ);
    ~ConfigurationImportFileFormat() override = default;

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
    EnumOptions<ConfigurationImportFileFormat::ConfigurationImportFormat> formats_;

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
    // Import DL_POLY coordinates through specified parser
    bool importDLPOLY(LineParser &parser, std::vector<Vec3<double>> &r);
    // Import EPSR ATO coordinates through specified parser
    bool importEPSR(LineParser &parser, std::vector<Vec3<double>> &r);
    // Import Moscito coordinates through specified parser
    bool importMoscito(LineParser &parser, std::vector<Vec3<double>> &r);
    // Import xyz coordinates through specified parser
    bool importXYZ(LineParser &parser, std::vector<Vec3<double>> &r);

    public:
    // Import coordinates using current filename and format
    bool importData(std::vector<Vec3<double>> &r, const ProcessPool *procPool = nullptr);
    // Import coordinates direct to configuration using current filename and format
    bool importData(Configuration *cfg, const ProcessPool *procPool = nullptr);
    // Import coordinates using supplied parser and current format
    bool importData(LineParser &parser, std::vector<Vec3<double>> &r);
    // Import coordinates direct to configuration using supplied parser and current format
    bool importData(LineParser &parser, Configuration *cfg);
};
