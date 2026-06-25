// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/integerHistogram1D.h"
#include "math/range.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

// Histogrammed Coordination Numbers
class HistogramCNNode : public Node
{
    public:
    HistogramCNNode(Graph *parentGraph);
    ~HistogramCNNode() override = default;

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
    // Target configuration
    Configuration *configuration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_;
    // Distance range for calculation
    Range distanceRange_{0.0, 5.0};
    // Histogrammed coordination numbers
    std::optional<IntegerHistogram1D> histogram_;
    Data1D cn_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &cn() const;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
