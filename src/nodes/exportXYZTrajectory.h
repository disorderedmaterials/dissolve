// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include <iostream>

// Forward Declarations
class Configuration;

class ExportXYZTrajectoryNode : public Node
{
    public:
    ExportXYZTrajectoryNode(Graph *parentGraph);
    ~ExportXYZTrajectoryNode() override = default;

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
    bool extended_;
    // Last read file position
    std::streampos filePosition_;
    // Target configuration
    Configuration *configuration_{nullptr};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
