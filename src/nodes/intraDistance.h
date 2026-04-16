// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "io/export/data1D.h"
#include "math/histogram1D.h"
#include "nodes/node.h"

// IntraDistance Node
class IntraDistanceNode : public Node
{
    public:
    IntraDistanceNode(Graph *parentGraph);
    ~IntraDistanceNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
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
    // Export target
    Data1DExportFileFormat exportFileAndFormat_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    /*
     * Getters
     */
    public:
    // Distance histogram AB
    const Data1D &rdfAB() const;
};
