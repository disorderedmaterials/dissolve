// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"
#include "templates/optionalRef.h"

// Forward Declarations
class Data2DBase;

// Data2D Export Formats
class Data2DExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum class Data2DExportFormat
    {
        Block,
        Cartesian
    };
    Data2DExportFileFormat(std::string_view filename = "", Data2DExportFormat format = Data2DExportFormat::Block);
    ~Data2DExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<Data2DExportFileFormat::Data2DExportFormat> formats_;

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
    // Export Data2D as simple block data
    bool exportBlock(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                     const Array2D<double> &values, OptionalReferenceWrapper<const Array2D<double>> errors = {});
    // Export Data2D as cartesian data
    bool exportCartesian(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                         const Array2D<double> &values, OptionalReferenceWrapper<const Array2D<double>> errors = {});

    public:
    // Export Data2D using current filename and format
    bool exportData(const Data2DBase &data);
};
