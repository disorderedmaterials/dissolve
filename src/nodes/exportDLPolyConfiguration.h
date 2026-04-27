// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"
#include <iostream>
#include <strstream>

// Forward Declarations
class Configuration;

class ExportDLPolyConfiguration : public Node
{
    public:
    ExportDLPolyConfiguration(Graph *parentGraph);
    ~ExportDLPolyConfiguration() override = default;

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

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
