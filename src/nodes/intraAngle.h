// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram1D.h"
#include "math/range.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

class IntraAngleNode : public Node
{
    public:
    IntraAngleNode(Graph *parentGraph);
    ~IntraAngleNode() override = default;

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
    Configuration *targetConfiguration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_, c_;
    // Range of A-B distance axis
    Range rangeAB_{0.0, 10.0};
    // Range of B-C distance axis
    Range rangeBC_{0.0, 10.0};
    // Range (min, max, binwidth) of angle axis
    Vector3 angleRange_{0.0, 180.0, 1.0};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Angle(ABC)
    std::optional<Histogram1D> intraAngleHist_;
    Data1D angleABC_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &intraAngleData() const;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
