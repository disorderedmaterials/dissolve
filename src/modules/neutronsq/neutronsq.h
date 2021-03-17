// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "data/structurefactors.h"
#include "io/import/data1d.h"
#include "module/module.h"

// Forward Declarations
class PartialSet;
class RDFModule;
class Weights;

// SQ Module
class NeutronSQModule : public Module
{
    public:
    NeutronSQModule();
    ~NeutronSQModule() override = default;

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
     * Initialisation
     */
    private:
    // Isotopologue information
    IsotopologueSet isotopologues_;
    // Exchangeable AtomTypes
    AtomTypeList exchangeableTypes_;
    // Reference F(Q) file and format
    Data1DImportFileFormat referenceFQ_;

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
