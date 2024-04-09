// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "math/ft.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

/*
 * Public Functions
 */

// Generate S(Q) from supplied g(r)
bool SQModule::calculateUnweightedSQ(const ProcessPool &procPool, const PartialSet &unweightedgr, PartialSet &unweightedsq,
                                     double qMin, double qDelta, double qMax, double rho, const WindowFunction &windowFunction,
                                     Function1DWrapper broadening)
{
    // Copy partial g(r) into our new S(Q) object - it should have been initialised already, so we will just check its size
    if (unweightedgr.nAtomTypes() != unweightedsq.nAtomTypes())
        return Messenger::error("SQModule::calculateUnweightedSQ - sizes of supplied partial sets are different.\n");

    // Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
    // Don't subtract 1.0 from the bound partials
    Timer timer;
    timer.start();
    dissolve::for_each_pair(
        ParallelPolicies::par, 0, unweightedgr.nAtomTypes(),
        [&](int n, int m)
        {
            // Total partial
            unweightedsq.partial(n, m).copyArrays(unweightedgr.partial(n, m));
            unweightedsq.partial(n, m) -= 1.0;
            Fourier::sineFT(unweightedsq.partial(n, m), 4.0 * PI * rho, qMin, qDelta, qMax, windowFunction, broadening);

            // Bound partial
            unweightedsq.boundPartial(n, m).copyArrays(unweightedgr.boundPartial(n, m));
            Fourier::sineFT(unweightedsq.boundPartial(n, m), 4.0 * PI * rho, qMin, qDelta, qMax, windowFunction, broadening);

            // Unbound partial
            unweightedsq.unboundPartial(n, m).copyArrays(unweightedgr.unboundPartial(n, m));
            unweightedsq.unboundPartial(n, m) -= 1.0;
            Fourier::sineFT(unweightedsq.unboundPartial(n, m), 4.0 * PI * rho, qMin, qDelta, qMax, windowFunction, broadening);
        });

    // Sum into total
    unweightedsq.formTotals(true);

    timer.stop();
    Messenger::print("Finished Fourier transform and summation of partial g(r) into partial S(Q) ({} elapsed).\n",
                     timer.totalTimeString());

    return true;
}
