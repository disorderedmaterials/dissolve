// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/export/trajectory.h"
#include "nodes/node.h"
#include <iostream>

// Forward Declarations
class Configuration;

class ExportTrajectoryNode : public Node
{
    public:
    ExportTrajectoryNode(Graph *parentGraph);
    ~ExportTrajectoryNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // File path
    std::string filePath_;
    // File format
    TrajectoryExportFileFormat::TrajectoryExportFormat format_{TrajectoryExportFileFormat::TrajectoryExportFormat::XYZ};
    // Last read file position (as int)
    std::streampos filePosition_;
    // Target configuration
    Configuration *configuration_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
