// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram1D.h"
#include "nodes/node.h"

class IntraDistanceNode : public Node
{
    public:
    IntraDistanceNode(Graph *parentGraph);
    ~IntraDistanceNode() override = default;

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
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_;
    // Range (min, max, delta) of distance axis
    Vector3 distanceRange_{0.0, 10.0, 0.05};
    // r(AB)
    std::optional<Histogram1D> histAB_;
    Data1D rdfAB_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &rdfAB() const;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
