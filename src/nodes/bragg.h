// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/timer.h"
#include "classes/atom.h"
#include "classes/box.h"
#include "classes/braggReflection.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/kVector.h"
#include "classes/neutronWeights.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "nodes/node.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"
#include "math/history.h"

// Bragg Module
class BraggNode : public Node
{
    public:
    BraggNode(Graph *parentGraph);
    ~BraggNode() override = default;

    std::string_view BraggNode::type() const override;
    std::string_view BraggNode::summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Number of historical data sets to combine into final reflection data
    std::optional<Number> averagingLength_{5};
    // Weighting scheme to use when averaging reflection data
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};
    // Bragg intensity scaling factor accounting for number of repeat units in Configuration
    Vector3i multiplicity_{1, 1, 1};
    // Resolution (bin width) in Q space to use when calculating Bragg reflections
    Number qDelta_{0.001};
    // Maximum Q value for Bragg calculation
    Number qMax_{1.0};
    // Minimum Q value for Bragg calculation
    Number qMin_{0.01};
    // Bragg reflections
    std::optional<BraggReflectionVector> braggReflections_;
    // Bragg reflections history
    History<BraggReflectionVector> braggReflectionHistory_;
    // Bragg partials
    std::optional<Array2D<Data1D>> braggPartials_;
    // Whether to save Bragg reflection data to disk
    bool saveReflections_{false};

    /*
     * Functions
     */
    public:
    // Calculate Bragg terms for specified Configuration
    bool calculateBraggTerms(std::vector<BraggReflection> &braggReflections, Configuration *cfg, const double qMin,
                             const double qDelta, const double qMax, Vector3i multiplicity);
    // Form partial and total reflection functions from calculated reflection data
    bool formReflectionFunctions(std::vector<BraggReflection> &braggReflections, Configuration *cfg,
                                 Array2D<Data1D> &braggPartials, const double qMin, const double qDelta, const double qMax);
    // Re-bin reflection data into supplied arrays
    bool reBinReflections(std::vector<BraggReflection> &braggReflections, Configuration *cfg, Array2D<Data1D> &braggPartials);

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
