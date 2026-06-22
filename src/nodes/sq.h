// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/function1D.h"
#include "math/history.h"
#include "math/windowFunction.h"
#include "nodes/node.h"

class SQNode : public Node
{
    public:
    SQNode(Graph *parentGraph);
    ~SQNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
     */
    private:
    // Unweighted g(r)
    PartialSet *unweightedGR_{nullptr};
    // Unweighted S(Q)
    std::optional<PartialSet> unweightedSQ_;
    // Historical unweighted S(Q)
    History<PartialSet> unweightedSQHistory_;
    // Number of historical partial sets to combine into final partials
    std::optional<Number> averagingLength_;
    // Broadening function to apply to S(Q)
    Function1DWrapper qBroadening_{Functions1D::Form::GaussianC2, {0.0, 0.02}};
    // Step size in Q for S(Q) calculation
    Number qDelta_{0.05};
    // Maximum Q for calculated S(Q)
    Number qMax_{30.0};
    // Minimum Q for calculated S(Q)
    Number qMin_{0.05};
    // Window function to use when Fourier-transforming reference S(Q) to g(r))
    WindowFunction::Form windowFunction_{WindowFunction::Form::None};

    public:
    // Returns the source configuration, belonging to the input GR node
    const Configuration *sourceConfiguration();
    // Returns the unweighted SQ
    const PartialSet &unweightedSQ() const;
    // Returns the unweighted GR
    const PartialSet &unweightedGR() const;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
