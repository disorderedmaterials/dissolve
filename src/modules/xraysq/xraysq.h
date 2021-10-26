// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "data/formfactors.h"
#include "data/structurefactors.h"
#include "io/import/data1d.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;
class RDFModule;
class XRayWeights;

// SQ Module
class XRaySQModule : public Module
{
    public:
    XRaySQModule();
    ~XRaySQModule() override = default;

    /*
     * Instances
     */
    public:
    // Create instance of this module
    Module *createInstance() const override;

    /*
     * Definition
     */
    public:
    // Return type of module
    std::string_view type() const override;
    // Return category for module
    std::string_view category() const override;
    // Return brief description of module
    std::string_view brief() const override;
    // Return the number of Configuration targets this Module requires
    int nRequiredTargets() const override;

    /*
     * Control
     */
    private:
    // Reference F(Q) file and format
    Data1DImportFileFormat referenceFQ_;
    // Set the minimum Q value to use when Fourier-transforming the data
    double referenceFTQMin_{0.0};
    // Set the maximum Q value to use when Fourier-transforming the data
    double referenceFTQMax_{0.0};
    // Set the spacing in r to use when generating the Fourier-transformed data
    double referenceFTDeltaR_{0.05};
    // Whether to save combined form factor weightings for atomtype pairs
    bool saveFormFactors_{false};
    // Whether to save weighted g(r) and G(r) to disk after calculation
    bool saveGR_{false};
    // Whether to save the reference data and its Fourier transform
    bool saveReference_{false};
    // Save representative G(r), obtained from Fourier transform of the calculated F(Q)
    bool saveRepresentativeGR_{false};
    // Whether to save weighted S(Q) and F(Q) to disk after calculation
    bool saveSQ_{false};

    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

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
    // Calculate weighted g(r) from supplied unweighted g(r) and Weights
    bool calculateWeightedGR(const PartialSet &unweightedgr, PartialSet &weightedgr, const XRayWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate weighted S(Q) from supplied unweighted S(Q) and Weights
    bool calculateWeightedSQ(const PartialSet &unweightedsq, PartialSet &weightedsq, const XRayWeights &weights,
                             StructureFactors::NormalisationType normalisation);
    // Calculate xray weights for relevant Configuration targets
    void calculateWeights(const RDFModule *rdfModule, XRayWeights &weights,
                          XRayFormFactors::XRayFormFactorData formFactors) const;

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
