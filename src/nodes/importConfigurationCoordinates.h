// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/import/coordinates.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

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
    CoordinateImportFileFormat::CoordinateImportFormat format_{CoordinateImportFileFormat::CoordinateImportFormat::XYZ};
    // Configuration
    Configuration *configuration_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};