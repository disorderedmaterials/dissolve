// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/averaging.h"
#include "math/function1D.h"
#include "module/module.h"

// Forward Declarations
class Dissolve;
class ModuleGroup;
class PartialSet;

// RDF Module
class GRModule : public Module
{
    public:
    GRModule();
    ~GRModule() override = default;

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
    EnumOptions<GRModule::PartialsMethod> partialsMethods();

    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
    // Number of historical partial sets to combine into final partials
    std::optional<int> averagingLength_{5};
    // Weighting scheme to use when averaging partials
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Bin width (spacing in r) to use
    double binWidth_{0.025};
    // Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop
    bool internalTest_{false};
    // Type of broadening to apply to intramolecular g(r)
    Functions::Function1DWrapper intraBroadening_{Functions::Function1D::Gaussian, {0.18}};
    // Degree of smoothing to apply
    std::optional<int> nSmooths_;
    // Calculation method for partials
    GRModule::PartialsMethod partialsMethod_{GRModule::AutoMethod};
    // Maximum r to calculate g(r) out to, unless UseHalfCellRange is true
    std::optional<double> requestedRange_;
    // Whether to save partials and total functions to disk
    bool save_{false};
    // Whether to save original (unbroadened) partials and total functions to disk
    bool saveOriginal_{false};

    /*
     * Functions
     */
    private:
    // Calculate partial g(r) in serial with simple double-loop
    bool calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet);
    // Calculate partial g(r) with optimised double-loop
    bool calculateGRSimple(const ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double rdfRange);
    // Calculate partial g(r) utilising Cell neighbour lists
    bool calculateGRCells(const ProcessPool &procPool, Configuration *cfg, PartialSet &partialSet, const double binWidth);

    public:
    // Calculate and return effective density based on target Configurations
    std::optional<double> effectiveDensity() const;
    // Calculate and return used species populations based on target Configurations
    std::vector<std::pair<const Species *, double>> speciesPopulations() const;
    // (Re)calculate partial g(r) for the specified Configuration
    bool calculateGR(GenericList &processingData, const ProcessPool &procPool, Configuration *cfg,
                     GRModule::PartialsMethod method, const double rdfRange, const double rdfBinWidth, bool &alreadyUpToDate);
    // Calculate smoothed/broadened partial g(r) from supplied partials
    static bool calculateUnweightedGR(const ProcessPool &procPool, Configuration *cfg, const PartialSet &originalgr,
                                      PartialSet &weightedgr, const Functions::Function1DWrapper intraBroadening,
                                      int smoothing);
    // Sum unweighted g(r) over the supplied Module's target Configurations
    static bool sumUnweightedGR(GenericList &processingData, const ProcessPool &procPool, std::string_view targetPrefix,
                                std::string_view parentPrefix, const std::vector<Configuration *> &parentCfgs,
                                PartialSet &summedUnweightedGR);
    // Test supplied PartialSets against each other
    static bool testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold);
    // Test calculated partial against supplied reference data
    static bool testReferencePartial(const PartialSet &partials, double testThreshold, const Data1D &testData,
                                     std::string_view typeIorTotal, std::string_view typeJ = "", std::string_view target = "");

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
