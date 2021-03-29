// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "math/pairbroadeningfunction.h"
#include "module/module.h"

// Forward Declarations
class Dissolve;
class ModuleGroup;
class PartialSet;

// RDF Module
class RDFModule : public Module
{
    public:
    RDFModule();
    ~RDFModule() override = default;

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
    protected:
    // Perform any necessary initialisation for the Module
    void initialise() override;

    public:
    // Partial Calculation Method enum
    enum PartialsMethod
    {
        AutoMethod,
        CellsMethod,
        SimpleMethod,
        TestMethod,
        nPartialsMethods
    };
    // Return enum option info for PartialsMethod
    EnumOptions<RDFModule::PartialsMethod> partialsMethods();

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;

    /*
     * Members / Functions
     */
    private:
    // Test data
    Data1DStore testData_;

    private:
    // Calculate partial g(r) in serial with simple double-loop
    bool calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet);
    // Calculate partial g(r) with optimised double-loop
    bool calculateGRSimple(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double rdfRange);
    // Calculate partial g(r) utilising Cell neighbour lists
    bool calculateGRCells(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth);
    bool calculateGRCellsParallelImpl(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth);
    bool calculateGRCellsSingleImpl(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth);

    public:
    // Calculate and return effective density for based on the target Configurations
    double effectiveDensity() const;
    // Calculate and return used species populations based on target Configurations
    std::vector<std::pair<const Species *, double>> speciesPopulations() const;
    // (Re)calculate partial g(r) for the specified Configuration
    bool calculateGR(GenericList &processingData, ProcessPool &procPool, Configuration *cfg, RDFModule::PartialsMethod method,
                     const double rdfRange, const double rdfBinWidth, bool &alreadyUpToDate);
    // Calculate smoothed/broadened partial g(r) from supplied partials
    static bool calculateUnweightedGR(ProcessPool &procPool, Configuration *cfg, const PartialSet &originalgr,
                                      PartialSet &weightedgr, PairBroadeningFunction &intraBroadening, int smoothing);
    // Sum unweighted g(r) over the supplied Module's target Configurations
    static bool sumUnweightedGR(GenericList &processingData, ProcessPool &procPool, Module *parentModule,
                                const RDFModule *rdfModule, PartialSet &summedUnweightedGR);
    // Sum unweighted g(r) over all Configurations targeted by the specified ModuleGroup
    static bool sumUnweightedGR(GenericList &processingData, ProcessPool &procPool, Module *parentModule,
                                ModuleGroup *moduleGroup, PartialSet &summedUnweightedGR);
    // Test supplied PartialSets against each other
    static bool testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold);
    // Test calculated partial against supplied reference data
    static bool testReferencePartial(const PartialSet &partials, double testThreshold, const Data1D &testData,
                                     std::string_view typeIorTotal, std::string_view typeJ = "", std::string_view target = "");
    // Test calculated vs reference data (two source sets)
    static bool testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partials,
                                      std::string_view prefix);
    // Test calculated vs reference data (two source sets)
    static bool testReferencePartials(const Data1DStore &testData, double testThreshold, const PartialSet &partialsA,
                                      std::string_view prefixA, const PartialSet &partialsB, std::string_view prefixB);

    /*
     * GUI Widget
     */
    public:
    // Return a new widget controlling this Module
    ModuleWidget *createWidget(QWidget *parent, Dissolve &dissolve) override;
};
