// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/isotopologueset.h"
#include "classes/partialset.h"
#include "data/structurefactors.h"
#include "io/import/data1d.h"
#include "math/windowfunction.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;
class RDFModule;
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
    public:
    // Return type of module
    std::string_view type() const override;

    /*
     * Control
     */
    private:
    // Exchangeable atom types
    std::vector<std::shared_ptr<AtomType>> exchangeable_;
    // Isotopologues to use in weighting
    IsotopologueSet isotopologueSet_;
    // Normalisation to apply to calculated total F(Q)
    StructureFactors::NormalisationType normalisation_{StructureFactors::NoNormalisation};
    // Reference F(Q) file and format
    Data1DImportFileFormat referenceFQ_;
    // Set the minimum Q value to use when Fourier-transforming the data
    double referenceFTQMin_{0.0};
    // Set the maximum Q value to use when Fourier-transforming the data
    double referenceFTQMax_{0.0};
    // Set the spacing in r to use when generating the Fourier-transformed data
    double referenceFTDeltaR_{0.05};
    // Normalisation to remove from reference total F(Q)
    StructureFactors::NormalisationType referenceNormalisation_{StructureFactors::NoNormalisation};
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
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Members / Functions
     */
    private:
    // Test data
    Data1DStore testData_;

    public:
    // Calculate weighted g(r) from supplied unweighted g(r) and neutron weights
    bool calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, NeutronWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate weighted S(Q) from supplied unweighted S(Q) and neutron weights
    bool calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, NeutronWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate neutron weights for relevant Configuration targets
    void calculateWeights(const RDFModule *rdfModule, NeutronWeights &weights) const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
