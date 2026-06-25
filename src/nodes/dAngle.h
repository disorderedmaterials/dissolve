// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/rangedVector3.h"
#include "nodes/node.h"

// Forward Declarations
class Configuration;

class DAngleNode : public Node
{
    public:
    DAngleNode(Graph *parentGraph);
    ~DAngleNode() override = default;

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
    // Whether to exclude correlations between B and C sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, binwidth) of B-C distance axis
    RangedVector3 distanceRange_{{0.0, 10.0, 0.05}, Vector3(0.0, 0.0, 1.0e-5)};
    // Range (min, max, binwidth) of angle axis
    RangedVector3 angleRange_{{0.0, 180.0, 1.0}, Vector3(0.0, 0.0, 1.0e-5)};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_, c_;
    // Distance histogram B-C
    std::optional<Histogram1D> distanceHistogramBC_;
    Data1D rdfBC_;
    // Angle histogram A-B-C
    std::optional<Histogram1D> angleHistogram_;
    Data1D angle_;
    // Distance-angle map
    std::optional<Histogram2D> distanceAngleMap_;
    Data2D dAngle_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &rdfBC() const;
    const Data1D &angle() const;
    const Histogram2D &distanceAngleMap() const;
    const Data2D &dAngle() const;

    /*
     * Processing
     */
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
