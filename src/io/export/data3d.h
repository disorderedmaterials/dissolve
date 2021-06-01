// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"
#include "templates/array3d.h"
#include "templates/optionalref.h"

// Forward Declarations
class Data3DBase;

// Data3D Export Formats
class Data3DExportFileFormat : public FileAndFormat
{
    public:
    // Available data formats
    enum Data3DExportFormat
    {
        BlockData3D,
        CartesianData3D,
        PDensData3D,
        nData3DExportFormats
    };
    Data3DExportFileFormat(std::string_view filename = "", Data3DExportFormat format = Data3DExportFileFormat::BlockData3D);
    ~Data3DExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<Data3DExportFileFormat::Data3DExportFormat> formats_;

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
    // Export Data3D as simple block data
    bool exportBlock(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                     const std::vector<double> &zAxis, const Array3D<double> &values,
                     OptionalReferenceWrapper<const Array3D<double>> errors = {});
    // Export Data3D as cartesian data
    bool exportCartesian(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                         const std::vector<double> &zAxis, const Array3D<double> &values,
                         OptionalReferenceWrapper<const Array3D<double>> errors = {});
    // Export Data3D as pdens data
    bool exportPDens(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &yAxis,
                     const std::vector<double> &zAxis, const Array3D<double> &values,
                     OptionalReferenceWrapper<const Array3D<double>> errors = {});

    public:
    // Export Data3D using current filename and format
    bool exportData(const Data3DBase &data);
};
