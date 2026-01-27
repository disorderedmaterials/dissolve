// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#define _USE_MATH_DEFINES
#include "math/mathFunc.h"

#include "base/timer.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "math/ft.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

#include <ranges>

// Generate S(Q) from supplied g(r)
bool SQModule::calculateUnweightedSQ(const PartialSet &unweightedgr, PartialSet &unweightedsq, double qMin, double qDelta,
                                     double qMax, double rho, const WindowFunction &windowFunction,
                                     Function1DWrapper broadening)
{
    // Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
    // Don't subtract 1.0 from the bound partials
    Timer timer;
    timer.start();

    // Full partials
    dissolve::for_each(ParallelPolicies::par, unweightedgr.partials().begin(), unweightedgr.partials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedsq.partials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           sq -= 1.0;
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction, broadening);
                       });

    // Bound partials
    dissolve::for_each(ParallelPolicies::par, unweightedgr.boundPartials().begin(), unweightedgr.boundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedsq.boundPartials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction, broadening);
                       });

    // Unbound partials
    dissolve::for_each(ParallelPolicies::par, unweightedgr.unboundPartials().begin(), unweightedgr.unboundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedsq.unboundPartials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           sq -= 1.0;
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction, broadening);
                       });

    // Sum into total
    unweightedsq.formTotals(true);

    timer.stop();
    Messenger::print("Finished Fourier transform and summation of partial g(r) into partial S(Q) ({} elapsed).\n",
                     timer.totalTimeString());

    return true;
}
