// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "io/import/data1D.h"
#include "nodes/node.h"

class Data1DImportFileFormatNode : public Node
{
    public:
    Data1DImportFileFormatNode(Graph *parentGraph);
    ~Data1DImportFileFormatNode() override = default;

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
    // X value from which to calculate average Y and remove
    std::optional<double> removeAverageFromX_;
    // Minimum x value to permit when reading data
    std::optional<double> xMin_;
    // Maximum x value to permit when reading data
    std::optional<double> xMax_;
    // Column index to use for x values
    int xColumn_{1};
    // Column index to use for y values
    int yColumn_{2};
    // Column index to use for error values
    int errorColumn_{0};
    // Remove a number of points from the start of the data
    int nPointsToRemove_{0};
    // Data1D import file format
    Data1DImportFileFormat data1DImportFileFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};