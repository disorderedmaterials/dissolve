// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "io/import/coordinates.h"
#include "nodes/node.h"
#include <memory>

class ImportConfigurationCoordinatesNode : public Node
{
    public:
    ImportConfigurationCoordinatesNode(Graph *parentGraph);
    ~ImportConfigurationCoordinatesNode() override = default;

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
    CoordinateImportFileFormat::CoordinateImportFormat format_;
    // Configuration
    Configuration *configuration_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};