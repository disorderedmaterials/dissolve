// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "math/histogram3D.h"
#include "nodes/node.h"

// SDF
class SDFNode : public Node
{
    public:
    SDFNode(Graph *parentGraph);
    ~SDFNode() override = default;

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
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{true};
    // Range along X axis
    Vector3 rangeX_{-10.0, 10.0, 0.5};
    // Range along Y axis
    Vector3 rangeY_{-10.0, 10.0, 0.5};
    // Range along Z axis
    Vector3 rangeZ_{-10.0, 10.0, 0.5};
    // SDF
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
    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
