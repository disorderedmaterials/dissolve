// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "classes/partialSet.h"
#include "data/structureFactors.h"
#include "io/import/data1D.h"
#include "math/windowFunction.h"
#include "module/module.h"
#include "nodes/gr/gr.h"
#include "nodes/graph.h"
#include "nodes/node.h"
#include "nodes/parameter.h"
#include <optional>

// Forward Declarations
class PartialSet;
class Data1D;

// Neutron SQ Node
class NeutronSQNode : public Node
{
    public:
    NeutronSQNode(Graph *parentGraph);
    ~NeutronSQNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
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
    // Full weights
    NeutronWeights weights_;
    // Map of named isotopologue weights
    std::map<std::string_view, double> namedWeights_{{"Ar36", 36}};
    // Exchangeable atom types
    std::vector<std::shared_ptr<AtomType>> exchangeable_;
    // Normalisation to apply to calculated total F(Q)
    StructureFactors::NormalisationType normaliseTo_{StructureFactors::NoNormalisation};
    // Reference F(Q) file and format
    Data1D *referenceData_{nullptr};
    // Minimum Q value to use when Fourier-transforming the data
    std::optional<double> referenceFTQMin_{0.3};
    // Maximum Q value to use when Fourier-transforming the data
    std::optional<double> referenceFTQMax_{30.0};
    // Spacing in r to use when generating the Fourier-transformed data
    double referenceFTDeltaR_{0.05};
    // Normalisation to remove from reference total F(Q)
    StructureFactors::NormalisationType referenceNormalisedTo_{StructureFactors::NoNormalisation};
    // Window function to use when Fourier transforming reference total F(Q) into g(r)
    WindowFunction::Form referenceWindowFunction_{WindowFunction::Form::Lorch0};
    // Save weighted g(r) and G(r)
    bool saveGR_{false};
    // Save the reference data and its Fourier transform
    bool saveReference_{false};
    // Save representative G(r), obtained from Fourier transform of the calculated F(Q)
    bool saveRepresentativeGR_{false};
    // Save weighted partial and total structure factors
    bool saveSQ_{false};

    /*
     * Functions
     */
    public:
    // Calculate weighted g(r)
    bool calculateWeightedGR();
    // Calculate weighted S(Q)
    bool calculateWeightedSQ();
    // Calculate neutron weights matrix
    void calculateWeights(const KeyedVector<const Species *, double> &realSpeciesPopulations);

    private:
    // Return value of weighted SQ, emplacing if optional not initialised
    PartialSet &weightedSQ();
    // Return value of weighted GR, emplacing if optional not initialised
    PartialSet &weightedGR();

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
