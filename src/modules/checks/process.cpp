// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/box.h"
#include "main/dissolve.h"
#include "modules/checks/checks.h"

// Run main processing
bool ChecksModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Perform simple checks for the target Configuration(s)
     *
     * This is a serial routine, with each process calculating the same values and comparing for consistency.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Loop over target Configurations
    for (auto *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        Messenger::print("Checks: Threshold for distance checks is {} Angstroms\n", distanceThreshold_);
        Messenger::print("Checks: Threshold for angle checks is {} degrees\n", angleThreshold_);

        auto &atoms = cfg->atoms();

        double actual, delta;
        bool ok;

        /*
         * Check Distances
         */

        // Loop over distances to check
        for (const auto &d : distances_)
        {
            actual = cfg->box()->minimumDistance(atoms[d.indices(0)].r(), atoms[d.indices(1)].r());
            delta = fabs(actual - d.value());
            ok = delta < distanceThreshold_;
            Messenger::print("Distance between Atoms {} and {} is {} Angstroms, and is {} (delta = {}, tolerance = {}).\n",
                             d.indices(0) + 1, d.indices(1) + 1, actual, ok ? "OK" : "NOT OK", delta, distanceThreshold_);

            // Check consistency between processes
            if (!procPool.allTrue(ok))
            {
                Messenger::error("Failed consistency check between processes.\n");
                return false;
            }
        }

        /*
         * Check Angles
         */

        // Loop over angles to check
        for (const auto &a : angles_)
        {
            actual = cfg->box()->angleInDegrees(atoms[a.indices(0)].r(), atoms[a.indices(1)].r(), atoms[a.indices(2)].r());
            delta = fabs(actual - a.value());
            ok = delta < angleThreshold_;
            Messenger::print("Angle between Atoms {}, {} and {} is {} degrees, and is {} (delta = {}, tolerance = {}).\n",
                             a.indices(0) + 1, a.indices(1) + 1, a.indices(2) + 1, actual, ok ? "OK" : "NOT OK", delta,
                             angleThreshold_);

            // Check consistency between processes
            if (!procPool.allTrue(ok))
            {
                Messenger::error("Failed consistency check between processes.\n");
                return false;
            }
        }
    }

    return true;
}
