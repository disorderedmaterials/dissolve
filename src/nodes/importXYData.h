// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/node.h"

class ImportXYDataNode : public Node
{
    public:
    ImportXYDataNode(Graph *parentGraph);
    ~ImportXYDataNode() override = default;

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
    std::optional<Number> errorColumn_;
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
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;

    public:
    // Read data specified
    static bool read(Data1D &data, std::string filePath, int xColumn = 1, int yColumn = 2, int errorColumn = 0,
                     bool histogram = false);
};
