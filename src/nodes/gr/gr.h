// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/configuration.h"
#include "classes/partialSet.h"
#include "classes/species.h"
#include "items/list.h"
#include "math/averaging.h"
#include "math/data1D.h"
#include "math/function1D.h"
#include "nodes/graph.h"
#include "nodes/node.h"
#include "nodes/number.h"
#include "nodes/parameter.h"
#include <vector>

class GRNode : public Node
{
    public:
    GRNode(Graph *parentGraph);
    ~GRNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    // Partial Calculation Method enum
    enum class PartialsMethod
    {
        AutoMethod,
        CellsMethod,
        SimpleMethod,
        TestMethod,
        nPartialsMethods
    };
    // Return enum option info for PartialsMethod
    static EnumOptions<PartialsMethod> partialsMethods();

    private:
    // Target configurations
    std::vector<Configuration *> targetConfigurations_;
    // Original g(r)
    std::optional<PartialSet> originalgr_;
    // Unweighted g(r)
    std::optional<PartialSet> unweightedGR_;
    // Summed unweighted g(r)
    std::optional<PartialSet> summedUnweightedGR_;
    // Number of historical partial sets to combine into final partials
    std::optional<Number> averagingLength_{5};
    // Weighting scheme to use when averaging partials
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Bin width (spacing in r) to use
    Number binWidth_{0.001};
    // Perform internal check of calculated partials against a set calculated by a simple unoptimised double-loop
    bool internalTest_{false};
    // Type of broadening to apply to intramolecular g(r)
    Function1DWrapper intraBroadening_{Functions1D::Form::Gaussian, {0.18}};
    // Degree of smoothing to apply
    std::optional<Number> nSmooths_;
    // Calculation method for partials
    PartialsMethod partialsMethod_{PartialsMethod::AutoMethod};
    // Summed atom types
    AtomTypeMix combinedAtomTypes_;
    // Maximum r to calculate g(r) out to, unless UseHalfCellRange is true
    std::optional<Number> requestedRange_;
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
    bool calculateGRSimple(Configuration *cfg, PartialSet &partialSet, const double rdfRange);
    // Calculate partial g(r) utilising Cell neighbour lists
    bool calculateGRCells(Configuration *cfg, PartialSet &partialSet, const double binWidth);

    public:
    // Get original g(r), constructing if empty
    PartialSet &originalGR(Configuration *cfg, const double rdfRange, const double rdfBinWidth);
    // Get unweighted g(r), constructing if empty
    PartialSet &unweightedGR();
    // Get summed unweighted g(r), constructing if empty
    PartialSet &summedUnweightedGR();
    // Calculate and return effective density based on target Configurations
    double effectiveDensity() const;
    // Calculate and return used species populations based on target Configurations
    SpeciesPopulations speciesPopulations() const;
    // (Re)calculate partial g(r) for the specified Configuration
    bool calculateGR(Configuration *cfg, PartialSet &originalgr, PartialsMethod method, const double rdfRange,
                     const double rdfBinWidth, bool &alreadyUpToDate);
    // Calculate smoothed/broadened partial g(r) from supplied partials
    bool calculateUnweightedGR(Configuration *cfg, const PartialSet &originalgr, PartialSet &weightedgr,
                               const Function1DWrapper intraBroadening, int smoothing);
    // Sum unweighted g(r) over the supplied Module's target Configurations
    bool sumUnweightedGR(std::string_view targetPrefix, std::string_view parentPrefix,
                         const std::vector<Configuration *> &parentCfgs, PartialSet &summedUnweightedGR);
    // Test supplied PartialSets against each other
    bool testReferencePartials(PartialSet &setA, PartialSet &setB, double testThreshold);
    // Test calculated partial against supplied reference data
    bool testReferencePartial(const PartialSet &partials, double testThreshold, const Data1D &testData,
                              std::string_view typeIorTotal, std::string_view typeJ = "", std::string_view target = "");

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};

EnumOptions<GRNode::PartialsMethod> getEnumOptions(GRNode::PartialsMethod);
