// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include <iostream>
#include <strstream>

// Forward Declarations
class Configuration;

class ExportCoordinatesNode : public Node
{
    public:
    ExportCoordinatesNode(Graph *parentGraph);
    ~ExportCoordinatesNode() override = default;

    enum class CoordinateExportFormat
    {
        XYZ,
        DLPOLY
    };

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;
    static EnumOptions<CoordinateExportFormat> formatType();

    /*
     * Data
     */
    private:
    // File path
    std::string filePath_;
    // File format
    CoordinateExportFormat format_;
    // Whether to tag (suffix) the filename with the current iteration index
    bool tagWithIteration_{false};
    // Last read file position (as int)
    std::streampos filePosition_;
    // Target configuration
    Configuration *configuration_{nullptr};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
    // Export as XYZ format
    NodeConstants::ProcessResult exportXYZ(std::string path);
    // Export as DLPoly format
    NodeConstants::ProcessResult exportDLPOLY(std::string path);
};

EnumOptions<ExportCoordinatesNode::CoordinateExportFormat> getEnumOptions(ExportCoordinatesNode::CoordinateExportFormat);
