// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/data1dstore.h"
#include "classes/partialset.h"
#include "math/averaging.h"
#include "math/function1d.h"
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
     * Definition
     */
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

    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
    // Number of historical partial sets to combine into final partials
    int averagingLength_{5};
    // Weighting scheme to use when averaging partials
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Bin width (spacing in r) to use
    double binWidth_{0.025};
    // Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop
    bool internalTest_{false};
    // Type of broadening to apply to intramolecular g(r)
    Functions::Function1DWrapper intraBroadening_{Functions::Function1D::Gaussian, {0.18}};
    // Degree of smoothing to apply
    int nSmooths_{0};
    // Calculation method for partials
    RDFModule::PartialsMethod partialsMethod_{RDFModule::AutoMethod};
    // Maximum r to calculate g(r) out to, unless UseHalfCellRange is true
    double requestedRange_{15.0};
    // Whether to save partials and total functions to disk
    bool save_{false};
    // Whether to use the maximal RDF range possible that avoids periodic images
    bool useHalfCellRange_{true};

    /*
     * Functions
     */
    private:
    // Calculate partial g(r) in serial with simple double-loop
    bool calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet);
    // Calculate partial g(r) with optimised double-loop
    bool calculateGRSimple(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double rdfRange);
    // Calculate partial g(r) utilising Cell neighbour lists
    bool calculateGRCells(ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth);

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
                                      PartialSet &weightedgr, const Functions::Function1DWrapper intraBroadening,
                                      int smoothing);
    // Sum unweighted g(r) over the supplied Module's target Configurations
    static bool sumUnweightedGR(GenericList &processingData, ProcessPool &procPool, std::string_view targetPrefix,
                                std::string_view parentPrefix, const std::vector<Configuration *> &parentCfgs,
                                PartialSet &summedUnweightedGR);
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
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
