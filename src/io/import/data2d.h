// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "io/fileandformat.h"
#include "templates/vector3.h"

// Forward Declarations
class Data2D;
class ProcessPool;
class Vec3DoubleKeyword;

// Data2D Import Formats
class Data2DImportFileFormat : public FileAndFormat
{
    public:
    // Available Data2D formats
    enum Data2DImportFormat
    {
        CartesianData2D,
        nData2DImportFormats
    };
    explicit Data2DImportFileFormat(std::string_view filename = "", Data2DImportFormat format = CartesianData2D);
    ~Data2DImportFileFormat() override = default;

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
    EnumOptions<Data2DImportFileFormat::Data2DImportFormat> formats_;

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
    bool importCartesian(LineParser &parser, Data2D &data);

    public:
    // Import Data2D using current filename and format
    bool importData(Data2D &data, ProcessPool *procPool = nullptr);
    // Import Data2D using supplied parser and current format
    bool importData(LineParser &parser, Data2D &data);
};
