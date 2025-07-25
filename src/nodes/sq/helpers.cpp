// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

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
    // Copy partial g(r) into our new S(Q) object - it should have been initialised already, so we will just check its size
    if (unweightedGR_->nAtomTypes() != unweightedSQ_->nAtomTypes())
    {
        error("SQNode::calculateUnweightedSQ - sizes of supplied partial sets are different.\n");
        return false;
    }

    auto rho = unweightedGR_->effectiveDensity();
    auto qMin = qMin_.asDouble(), qDelta = qDelta_.asDouble(), qMax = qMax_.asDouble();

    // Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
    // Don't subtract 1.0 from the bound partials
    Timer timer;
    timer.start();
    dissolve::for_each_pair(ParallelPolicies::par, unweightedGR_->nAtomTypes(),
                            [&](int n, int m)
                            {
                                // Total partial
                                unweightedSQ_->partial(n, m).copyArrays(unweightedGR_->partial(n, m));
                                unweightedSQ_->partial(n, m) -= 1.0;
                                Fourier::sineFT(unweightedSQ_->partial(n, m), 4.0 * M_PI * rho, qMin, qDelta, qMax,
                                                windowFunction_, qBroadening_);

                                // Bound partial
                                unweightedSQ_->boundPartial(n, m).copyArrays(unweightedGR_->boundPartial(n, m));
                                Fourier::sineFT(unweightedSQ_->boundPartial(n, m), 4.0 * M_PI * rho, qMin, qDelta, qMax,
                                                windowFunction_, qBroadening_);

                                // Unbound partial
                                unweightedSQ_->unboundPartial(n, m).copyArrays(unweightedGR_->unboundPartial(n, m));
                                unweightedSQ_->unboundPartial(n, m) -= 1.0;
                                Fourier::sineFT(unweightedSQ_->unboundPartial(n, m), 4.0 * M_PI * rho, qMin, qDelta, qMax,
                                                windowFunction_, qBroadening_);
                            });

    // Sum into total
    unweightedSQ_->formTotals(true);

    timer.stop();
    message("Finished Fourier transform and summation of partial g(r) into partial S(Q) ({} elapsed).\n",
            timer.totalTimeString());

    return true;
}
