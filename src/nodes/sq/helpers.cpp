// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/mathFunc.h"

#include "base/timer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "math/ft.h"
#include "modules/sq/sq.h"
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
