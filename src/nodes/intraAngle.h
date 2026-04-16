// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "io/export/data1D.h"
#include "math/histogram1D.h"
#include "nodes/node.h"

// IntraAngle Node
class IntraAngleNode : public Node
{
    public:
    IntraAngleNode(Graph *parentGraph);
    ~IntraAngleNode() override = default;

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
    SpeciesSites a_, b_, c_;
    // Range of A-B distance axis
    Range rangeAB_{0.0, 10.0};
    // Range of B-C distance axis
    Range rangeBC_{0.0, 10.0};
    // Range (min, max, binwidth) of angle axis
    Vector3 angleRange_{0.0, 180.0, 1.0};
    // Angle(ABC)
    std::optional<Histogram1D> intraAngleHist_;
    Data1D angleABC_;
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
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
    // Intramolecular angle histogram
    const Data1D &intraAngleData() const;
};
