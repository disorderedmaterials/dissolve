// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "classes/partialSet.h"
#include "data/structureFactors.h"
#include "math/windowFunction.h"
#include "nodes/node.h"
#include <optional>

class NeutronSQNode : public Node
{
    public:
    NeutronSQNode(Graph *parentGraph);
    ~NeutronSQNode() override = default;

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
    // Unweighted S(Q)
    PartialSet *unweightedSQ_{nullptr};
    // Unweighted g(r)
    PartialSet *unweightedGR_{nullptr};
    // Weighted S(Q)
    std::optional<PartialSet> weightedSQ_;
    // Weighted g(r)
    std::optional<PartialSet> weightedGR_;
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
    std::optional<Number> referenceFTQMin_{0.5};
    // Maximum Q value to use when Fourier-transforming the data
    std::optional<Number> referenceFTQMax_{30.0};
    // Spacing in r to use when generating the Fourier-transformed data
    Number referenceFTDeltaR_{0.05};
    // Normalisation to remove from reference total F(Q)
    StructureFactors::NormalisationType referenceNormalisedTo_{StructureFactors::NoNormalisation};
    // Window function to use when Fourier transforming reference total F(Q) into g(r)
    WindowFunction::Form referenceWindowFunction_{WindowFunction::Form::Lorch0};

    public:
    // Clear any local data
    void clearData() override;
    // Return neutron weights
    NeutronWeights weights() const;
    // Returns the unweighted SQ
    const PartialSet *unweightedSQ() const;
    // Returns the unweighted GR
    const PartialSet *unweightedGR() const;
    // Returns the isotopologues
    const IsotopologueSet &isotopologues() const;
    // Returns the source configuration, belonging to the input SQ node
    const Configuration *sourceConfiguration();

    /*
     * Processing
     */
    private:
    // Calculate weighted g(r)
    bool calculateWeightedGR(const NeutronWeights &weights);
    // Calculate weighted S(Q)
    bool calculateWeightedSQ(const NeutronWeights &weights);

    protected:
    // Perform processing
    NodeConstants::ProcessResult process() override;
};
