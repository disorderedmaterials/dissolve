// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/box.h"
#include "classes/braggReflection.h"
#include "classes/configuration.h"
#include "classes/kVector.h"
#include "main/dissolve.h"
#include "math/history.h"
#include "nodes/node.h"

// Bragg Module
class BraggNode : public Node
{
    public:
    BraggNode(Graph *parentGraph);
    ~BraggNode() override = default;

    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Number of historical data sets to combine into final reflection data
    std::optional<Number> averagingLength_{5};
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
    // Unweighted S(Q)
    PartialSet *unweightedSQ_{nullptr};
    // Broadening function to apply to Bragg S(Q)
    Function1DWrapper braggQBroadening_{Functions1D::Form::GaussianC2, {0.0, 0.02}};
    // Whether to save Bragg reflection data to disk
    bool saveReflections_{false};

    /*
     * Functions
     */
    public:
    // Calculate Bragg terms for specified Configuration
    bool calculateBraggTerms();
    // Form partial and total reflection functions from calculated reflection data
    bool formReflectionFunctions();
    // Re-bin reflection data into supplied arrays
    bool reBinReflections();

    // Get reflections data
    const std::vector<BraggReflection> &braggReflections();

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
