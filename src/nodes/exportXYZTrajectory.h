// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

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
    // Return type of the node
    std::string_view type() const override;
    // Return short summary of the node's purpose
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // File path
    std::string filePath_;
    // File format
    bool extended_{false};
    // Last read file position
    std::streampos filePosition_;
    // Target configuration
    Configuration *configuration_{nullptr};

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
