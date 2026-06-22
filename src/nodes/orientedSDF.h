// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram3D.h"
#include "nodes/node.h"

// OrientedSDF
class OrientedSDFNode : public Node
{
    public:
    OrientedSDFNode(Graph *parentGraph);
    ~OrientedSDFNode() override = default;

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
    Configuration *configuration_{nullptr};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_;
    // Axes to use for sites
    Site::SiteAxis axisA_{Site::SiteAxis::XAxis}, axisB_{Site::SiteAxis::XAxis};
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{true};
    // Range along X axis
    Vector3 rangeX_{-10.0, 10.0, 0.5};
    // Range along Y axis
    Vector3 rangeY_{-10.0, 10.0, 0.5};
    // Range along Z axis
    Vector3 rangeZ_{-10.0, 10.0, 0.5};
    // Axis angle range requirement to permit binning into SDF
    Range angleRange_{-15.0, 15.0};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // OrientedSDF
    std::optional<Histogram3D> histogram_;
    Data3D sdf_;

    public:
    // Clear any local data
    void clearData() override;
    // Temporary accessors to data for testing
    const Data3D &sdf() const;

    /*
     * Processing
     */
    private:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
