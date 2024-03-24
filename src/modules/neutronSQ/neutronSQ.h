// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/isotopologueSet.h"
#include "classes/partialSet.h"
#include "data/structureFactors.h"
#include "io/import/data1D.h"
#include "math/windowFunction.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;
class GRModule;
class SQModule;

// Neutron SQ Module
class NeutronSQModule : public Module
{
    public:
    NeutronSQModule();
    ~NeutronSQModule() override = default;

    /*
     * Definition
     */
    private:
    // Exchangeable atom types
    std::vector<std::shared_ptr<AtomType>> exchangeable_;
    // Isotopologues to use in weighting
    IsotopologueSet isotopologueSet_;
    // Normalisation to apply to calculated total F(Q)
    StructureFactors::NormalisationType normaliseSumTo_{StructureFactors::NoNormalisation};
    // Reference F(Q) file and format
    Data1DImportFileFormat referenceFQ_;
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
    // Source module for calculation
    const SQModule *sourceSQ_{nullptr};

    public:
    // Return file and format for reference total F(Q)
    const Data1DImportFileFormat &referenceFQFileAndFormat();

    /*
     * Functions
     */
    public:
    // Calculate weighted g(r) from supplied unweighted g(r) and neutron weights
    bool calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, NeutronWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate weighted S(Q) from supplied unweighted S(Q) and neutron weights
    bool calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, NeutronWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate neutron weights for relevant Configuration targets
    void calculateWeights(const GRModule *rdfModule, NeutronWeights &weights) const;

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;

    public:
    // Set target data
    void setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                    const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap) override;
    // Run set-up stage
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
