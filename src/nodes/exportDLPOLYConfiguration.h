// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include <iostream>
#include <sstream>

// Forward Declarations
class Configuration;

class ExportDLPOLYConfigurationNode : public Node
{
    public:
    ExportDLPOLYConfigurationNode(Graph *parentGraph);
    ~ExportDLPOLYConfigurationNode() override = default;

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
    // Whether to tag (suffix) the filename with the current iteration index
    bool tagWithIteration_{false};
    // Target configuration
    Configuration *configuration_{nullptr};
    // Iteration count
    int iteration_{1};

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
