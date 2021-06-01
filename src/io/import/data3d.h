// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Data3D;
class ProcessPool;

// Data3D Import Formats
class Data3DImportFileFormat : public FileAndFormat
{
    public:
    // Available Data3D formats
    enum class Data3DImportFormat
    {
        Cartesian
    };
    explicit Data3DImportFileFormat(std::string_view filename = "", Data3DImportFormat format = Data3DImportFormat::Cartesian);
    ~Data3DImportFileFormat() override = default;

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
    EnumOptions<Data3DImportFileFormat::Data3DImportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

    /*
     * Data Import
     */
    private:
    // Import cartesian data from supplied parser
    bool importCartesian(LineParser &parser, Data3D &data);

    public:
    // Import Data3D using current filename and format
    bool importData(Data3D &data, ProcessPool *procPool = nullptr);
    // Import Data3D using supplied parser and current format
    bool importData(LineParser &parser, Data3D &data);
};
