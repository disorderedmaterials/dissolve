// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "classes/partialSet.h"
#include "classes/xRayWeights.h"
#include "data/formFactors.h"
#include "data/structureFactors.h"
#include "math/windowFunction.h"
#include "nodes/node.h"
#include <optional>

// Forward Declarations
class Configuration;

class XRaySQNode : public Node
{
    public:
    XRaySQNode(Graph *parentGraph);
    ~XRaySQNode() override = default;

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
    // Atomic form factors to use for weighting
    XRayFormFactors::XRayFormFactorData formFactors_{XRayFormFactors::WaasmaierKirfel1995};
    // XRay weights
    XRayWeights weights_;
    // Unweighted S(Q)
    PartialSet *unweightedSQ_{nullptr};
    // Unweighted g(r)
    PartialSet *unweightedGR_{nullptr};
    // Representative g(r) calculated from FT of total weighted F(Q)
    Data1D representativeGR_;
    // Isotopologues to use for constructing weights matrix
    IsotopologueSet isotopologues_;
    // Normalisation to apply to calculated total F(Q)
    StructureFactors::NormalisationType normaliseTo_{StructureFactors::NoNormalisation};
    // Reference F(Q) data
    std::optional<Data1D> referenceFQ_;
    // Reference G(r) data from FT of reference F(Q)
    Data1D referenceGR_;
    // Minimum Q value to use when Fourier-transforming the data
    std::optional<Number> referenceFTQMin_{0.3};
    // Maximum Q value to use when Fourier-transforming the data
    std::optional<Number> referenceFTQMax_{30.0};
    // Spacing in r to use when generating the Fourier-transformed data
    Number referenceFTDeltaR_{0.05};
    // Normalisation to remove from reference total F(Q)
    StructureFactors::NormalisationType referenceNormalisedTo_{StructureFactors::NoNormalisation};
    // Window function to use when Fourier transforming reference total F(Q) into g(r)
    WindowFunction::Form referenceWindowFunction_{WindowFunction::Form::Lorch0};
    // Weighted S(Q)
    std::optional<PartialSet> weightedSQ_;
    // Weighted g(r)
    std::optional<PartialSet> weightedGR_;

    public:
    // Returns the unweighted SQ
    const PartialSet *unweightedSQ() const;
    // Returns the unweighted GR
    const PartialSet *unweightedGR() const;
    // Returns the source configuration, belonging to the input SQ node
    const Configuration *sourceConfiguration();
    // Return xRay weights
    const XRayWeights &weights() const;

    /*
     * Processing
     */
    private:
    // Set up reference data
    bool setUpReferenceData();
    // Calculate weighted g(r) from supplied unweighted g(r) and Weights
    bool calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
    bool calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                             StructureFactors::NormalisationType normalisation);

    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};