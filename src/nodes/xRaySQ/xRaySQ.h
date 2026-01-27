// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/exchangeables.h"
#include "classes/isotopologueSet.h"
#include "classes/partialSet.h"
#include "data/structureFactors.h"
#include "math/windowFunction.h"
#include "nodes/gr/gr.h"
#include "nodes/node.h"
#include <optional>
#include "classes/xRayWeights.h"
#include "data/formFactors.h"
#include "io/import/data1D.h"
#include "math/windowFunction.h"

// Forward Declarations
class PartialSet;
class XRayWeights;

class XRaySQNode : public Node
{
    public:
    XRaySQNode(Graph *parentGraph);
    ~XRaySQNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
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
    // Weighted S(Q)
    std::optional<PartialSet> weightedSQ_;
    // Weighted g(r)
    std::optional<PartialSet> weightedGR_;
    // Representative g(r) calculated from FT of total weighted F(Q)
    Data1D representativeGR_;
    // Isotopologues to use for constructing weights matrix
    IsotopologueSet isotopologues_;
    // Exchangeable atom types
    Exchangeables exchangeables_;
    // Normalisation to apply to calculated total F(Q)
    StructureFactors::NormalisationType normaliseTo_{StructureFactors::NoNormalisation};
    // Reference F(Q) data
    std::optional<Data1D> referenceFQ_;
    // Reference G(r) data from FT of reference F(Q)
    Data1D referenceGR_;
    // Minimum Q value to use when Fourier-transforming the data
    std::optional<double> referenceFTQMin_{0.5};
    // Maximum Q value to use when Fourier-transforming the data
    std::optional<double> referenceFTQMax_{30.0};
    // Spacing in r to use when generating the Fourier-transformed data
    double referenceFTDeltaR_{0.05};
    // Normalisation to remove from reference total F(Q)
    StructureFactors::NormalisationType referenceNormalisedTo_{StructureFactors::NoNormalisation};
    // Window function to use when Fourier transforming reference total F(Q) into g(r)
    WindowFunction::Form referenceWindowFunction_{WindowFunction::Form::Lorch0};
    // Whether to save combined form factor weightings for atomtype pairs
    bool saveFormFactors_{false};
    // Save weighted g(r) and G(r)
    bool saveGR_{false};
    // Save the reference data and its Fourier transform
    bool saveReference_{false};
    // Save representative G(r), obtained from Fourier transform of the calculated F(Q)
    bool saveRepresentativeGR_{false};
    // Save weighted partial and total structure factors
    bool saveSQ_{false};

    public:
    // Return file and format for reference total F(Q)
    const Data1DImportFileFormat &referenceFQFileAndFormat();

    /*
     * Functions
     */
    public:
    // Return xRay weights
    const XRayWeights &weights() const;
    // Calculate weighted g(r) from supplied unweighted g(r) and Weights
    bool calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
    bool calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                             StructureFactors::NormalisationType normalisation);

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
