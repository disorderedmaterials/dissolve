// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/configuration.h"
#include "classes/histogramSet.h"
#include "classes/partialSet.h"
#include "classes/species.h"
#include "items/list.h"
#include "math/function1D.h"
#include "math/history.h"
#include "nodes/node.h"
#include "nodes/number.h"
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
    Configuration *targetConfiguration_{nullptr};
    // Raw simulation g(r)
    std::optional<PartialSet> rawGR_;
    // Historical raw g(r)
    History<PartialSet> rawGRHistory_;
    // Unweighted g(r)
    std::optional<PartialSet> unweightedGR_;
    // Number of historical partial sets to combine into final partials
    std::optional<Number> averagingLength_{5};
    // Bin width (spacing in r) to use
    Number binWidth_{0.001};
    // Type of broadening to apply to intramolecular g(r)
    Function1DWrapper intraBroadening_{Functions1D::Form::Gaussian, {0.18}};
    // Degree of smoothing to apply
    std::optional<Number> nSmooths_;
    // Calculation method for partials
    PartialsMethod partialsMethod_{PartialsMethod::AutoMethod};
    // Maximum r to calculate g(r) out to, unless UseHalfCellRange is true
    std::optional<Number> requestedRange_;
    // Whether to save partials and total functions to disk
    bool save_{false};
    // Whether to save raw partials and total functions to disk
    bool saveRaw_{false};
    // Histograms for RDF calculation
    std::optional<HistogramSet> histograms_;

    /*
     * Functions
     */
    private:
    // Calculate partial g(r) in serial with simple double-loop
    bool calculateGRTestSerial(const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT);
    // Calculate partial g(r) with optimised double-loop
    bool calculateGRSimple(const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT);
    // Calculate partial g(r) utilising Cell neighbour lists
    bool calculateGRCells(double grRange, const Array2D<typename std::map<std::string, Histogram1D>::iterator> &fullLUT);
    // Calculate RDF from raw histogram
    void calculateRDF(Data1D &gr, const Histogram1D &histogram, double boxVolume, int nCentres, int nSurrounding,
                      double multiplier);

    public:
    // Calculate raw partials
    bool calculateRawGR(const double grRange, bool &alreadyUpToDate);
    // Calculate smoothed/broadened partial g(r) from supplied partials
    bool calculateUnweightedGR();
    // Test supplied PartialSets against each other
    bool testReferencePartials(const std::vector<const AtomType *> &types, PartialSet &setA, PartialSet &setB,
                               double testThreshold);

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};

EnumOptions<GRNode::PartialsMethod> getEnumOptions(GRNode::PartialsMethod);
