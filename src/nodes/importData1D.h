// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "io/import/data1D.h"
#include "nodes/node.h"
#include <memory>

class ImportData1DNode : public Node
{
    public:
    ImportData1DNode(Graph *parentGraph);
    ~ImportData1DNode() override = default;

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
    // Whether file format is histogram
    bool histogram_;
    // Column index of data X values
    Number xColumn_{1};
    // Column index of data Y values
    Number yColumn_{2};
    // Column index of data error values
    Number errorColumn_{0};
    // Remove average from X
    std::optional<Number> removeAverageFromX_;
    // Minimum X value
    std::optional<Number> xMin_;
    // Maximum X value
    std::optional<Number> xMax_;
    // Number of points to remove from beginning of data
    Number nPointsToRemove_{0};
    // Imported data
    std::optional<Data1D> data_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    public:
    // Read data specified
    static bool read(Data1D &data, std::string filePath, int xColumn, int yColumn, int errorColumn, bool histogram);
};
