// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "io/fileAndFormat.h"
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
    enum class Data2DImportFormat
    {
        Cartesian
    };
    explicit Data2DImportFileFormat(std::string_view filename = "", Data2DImportFormat format = Data2DImportFormat::Cartesian);
    ~Data2DImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    private:
    // Min, max, and delta to assume for x axis
    Vec3<double> xAxis_{0.0, 10.0, 0.1};
    // Min, max, and delta to assume for y axis
    Vec3<double> yAxis_{0.0, 10.0, 0.1};

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
    bool importData(Data2D &data, const ProcessPool *procPool = nullptr);
    // Import Data2D using supplied parser and current format
    bool importData(LineParser &parser, Data2D &data);
};
