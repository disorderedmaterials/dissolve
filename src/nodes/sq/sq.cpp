// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/sq/sq.h"

SQNode::SQNode(Graph *parentGraph)
    : Node(parentGraph), unweightedSQHistory_(
                             [&]()
                             {
                                 PartialSet p;
                                 p.initialise(*unweightedGR_);
                                 return p;
                             })
{
    // Inputs
    addInput<PartialSet *>("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Outputs
    addOptionalPointerOutput<PartialSet>("UnweightedSQ", "Unweighted partials for target configuration", unweightedSQ_);
    addOutput<PartialSet *>("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_);

    // Options
    addOption<Number>("QMin", "Minimum Q for calculated S(Q)", qMin_);
    addOption<Number>("QMax", "Maximum Q for calculated S(Q)", qMax_);
    addOption<Number>("QDelta", "Step size in Q for S(Q) calculation", qDelta_);
    addOption<Function1DWrapper>("QBroadening", "Instrument broadening function to apply when calculating S(Q)", qBroadening_);
    addOption<WindowFunction::Form>(
        "WindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)", windowFunction_);
    addOption<std::optional<Number>>("Averaging", "Number of historical partial sets to combine into final partials",
                                     averagingLength_);
    addOption<bool>("Save", "Whether to save partials to disk after calculation", save_);

    // Serialisables
    addSerialisable("unweightedSQ", unweightedSQ_);
    addSerialisable("unweightedSQ", unweightedSQHistory_);
}

std::string_view SQNode::type() const { return "SQ"; }

std::string_view SQNode::summary() const { return "Unweighted structure factors."; }
