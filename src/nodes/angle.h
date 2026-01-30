// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/speciesSites.h"
#include "io/export/data1D.h"
#include "io/export/data2D.h"
#include "math/histogram1D.h"
#include "math/histogram2D.h"
#include "math/histogram3D.h"
#include "math/rangedVector3.h"
#include "nodes/node.h"

class AngleNode : public Node
{
    public:
    AngleNode(Graph *parentGraph);
    ~AngleNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *configuration_{nullptr};
    // Whether to exclude correlations between A and B sites on the same molecule
    bool excludeSameMoleculeAB_{false};
    // Whether to exclude correlations between B and C sites on the same molecule
    bool excludeSameMoleculeBC_{false};
    // Whether to exclude correlations between A and C sites on the same molecule
    bool excludeSameSiteAC_{false};
    // Range (min, max, binwidth) of A-B distance axis
    RangedVector3 rangeAB_{{0.0, 10.0, 0.05}, Vector3(0.0, 0.0, 1.0e-5)};
    // Range (min, max, binwidth) of B-C distance axis
    RangedVector3 rangeBC_{{0.0, 10.0, 0.05}, Vector3(0.0, 0.0, 1.0e-5)};
    // Range (min, max, binwidth) of angle axis
    RangedVector3 angleRange_{{0.0, 180.0, 1.0}, Vector3(0.0, 0.0, 1.0e-5)};
    // Whether the angular range should be considered symmetric about 90
    bool symmetric_{false};
    // Target SpeciesSite definitions
    SpeciesSites a_, b_, c_;
    // RDF(A-B)
    std::optional<Histogram1D> distanceHistogramAB_;
    Data1D rdfAB_;
    // RDF(B-C)
    std::optional<Histogram1D> distanceHistogramBC_;
    Data1D rdfBC_;
    // Angle(A-B-C)
    std::optional<Histogram1D> angleHistogramABC_;
    Data1D angleABC_;
    // DAngle((A-B)-C)
    std::optional<Histogram2D> dAngleHistogramAB_;
    Data2D dAngleAB_;
    // DAngle(A-(B-C))
    std::optional<Histogram2D> dAngleHistogramBC_;
    Data2D dAngleBC_;
    // DDAngle(A-B-C)
    std::optional<Histogram3D> dDAngleHistogramABC_;
    Data3D dDAngleABC_;
    // Export targets
    Data1DExportFileFormat exportFileAndFormatAB_, exportFileAndFormatBC_, exportFileAndFormatAngle_;
    Data2DExportFileFormat exportFileAndFormatDAngleAB_, exportFileAndFormatDAngleBC_;

    /*
     * Data
     */
    public:
    // Clear any local data
    void clearData() override;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
