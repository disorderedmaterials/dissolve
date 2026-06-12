// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/timer.h"
#include "math/ft.h"
#include "math/mathFunc.h"
#include "nodes/edge.h"
#include "nodes/gr/gr.h"
#include "nodes/sq/sq.h"
#include "templates/algorithms.h"

/*
 * Public Functions
 */

// Generate S(Q) from supplied g(r)
bool SQNode::calculateUnweightedSQ()
{
    auto rho = unweightedGR_->effectiveDensity();
    auto qMin = qMin_.asDouble(), qDelta = qDelta_.asDouble(), qMax = qMax_.asDouble();

    // Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
    // Don't subtract 1.0 from the bound partials
    Timer timer;
    timer.start();

    // Full partials
    dissolve::for_each(ParallelPolicies::par, unweightedGR_->partials().begin(), unweightedGR_->partials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedSQ_->partials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           sq -= 1.0;
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction_, qBroadening_);
                       });

    // Bound partials
    dissolve::for_each(ParallelPolicies::par, unweightedGR_->boundPartials().begin(), unweightedGR_->boundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedSQ_->boundPartials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction_, qBroadening_);
                       });

    // Unbound partials
    dissolve::for_each(ParallelPolicies::par, unweightedGR_->unboundPartials().begin(), unweightedGR_->unboundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedSQ_->unboundPartials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           sq -= 1.0;
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction_, qBroadening_);
                       });

    // Sum into total
    unweightedSQ_->formTotals(true);

    timer.stop();
    message("Finished Fourier transform and summation of partial g(r) into partial S(Q) ({} elapsed).\n",
            timer.totalTimeString());

    return true;
}

// Returns the unweighted SQ
const PartialSet &SQNode::unweightedSQ() const { return *unweightedSQ_; }

// Returns the unweighted GR
const PartialSet &SQNode::unweightedGR() const { return *unweightedGR_; }

// Returns the source configuration, belonging to the input GR node
const Configuration *SQNode::sourceConfiguration()
{
    auto cfgInputEdge = inputEdges().find("UnweightedGR");

    if (cfgInputEdge == inputEdges().end())
    {
        error("Could not find a valid input 'UnweightedGR' associated with this node ({})", name());
        return nullptr;
    }

    auto &cfgSourceNode = cfgInputEdge->second[0]->sourceNode();
    auto grNode = static_cast<GRNode *>(&cfgSourceNode);

    return grNode->getInputValue<Configuration *>("Configuration");
}