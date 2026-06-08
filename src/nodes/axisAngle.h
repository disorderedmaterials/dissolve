// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/rangedVector3.h"
#include "nodes/node.h"

// Forward Declarations
class SpeciesSite;

// Axis Angle
class AxisAngleNode : public Node
{
    public:
    AxisAngleNode(Graph *parentGraph);
    ~AxisAngleNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *configuration_{nullptr};
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, binwidth) of distance axis
    RangedVector3 distanceRange_{{0.0, 10.0, 0.1}, Vector3{0.0, 0.0, 1.0e-5}};
    // Range (min, max, binwidth) of angle axis
    RangedVector3 angleRange_{{0.0, 180.0, 1.0}, Vector3(0.0, 0.0, 1.0e-5)};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_;
    // Axes to use for sites
    Site::SiteAxis axisA_{Site::SiteAxis::ZAxis}, axisB_{Site::SiteAxis::ZAxis};

    /*
     * Data
     */
    private:
    // RDF
    std::optional<Histogram1D> distanceHistogram_;
    Data1D rdf_;
    // Axis Angle
    std::optional<Histogram1D> axisAngleHistogram_;
    Data1D axisAngle_;
    // DAngle Map
    std::optional<Histogram2D> dAxisAngleHistogram_;
    Data2D dAxisAngle_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data1D &axisAngle() const;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
