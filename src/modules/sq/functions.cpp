// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "math/ft.h"
#include "modules/sq/sq.h"

/*
 * Public Functions
 */

// Generate S(Q) from supplied g(r)
bool SQModule::calculateUnweightedSQ(ProcessPool &procPool, Configuration *cfg, const PartialSet &unweightedgr,
                                     PartialSet &unweightedsq, double qMin, double qDelta, double qMax, double rho,
                                     const WindowFunction &windowFunction, const BroadeningFunction &broadening)
{
    // Copy partial g(r) into our new S(Q) object - it should have been initialised already, so we will just check its size
    if (unweightedgr.nAtomTypes() != unweightedsq.nAtomTypes())
        return Messenger::error("SQModule::calculateUnweightedSQ - sizes of supplied partial sets are different.\n");

    // Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
    // Don't subtract 1.0 from the bound partials
    // TODO Parallelise this
    procPool.resetAccumulatedTime();
    Timer timer;
    timer.start();
    auto nTypes = unweightedgr.nAtomTypes();
    for (int n = 0; n < nTypes; ++n)
    {
        for (int m = n; m < nTypes; ++m)
        {
            // Total partial
            unweightedsq.partial(n, m).copyArrays(unweightedgr.partial(n, m));
            unweightedsq.partial(n, m).values() -= 1.0;
            if (!Fourier::sineFT(unweightedsq.partial(n, m), 4.0 * PI * rho, qMin, qDelta, qMax, windowFunction, broadening))
                return false;

            // Bound partial
            unweightedsq.boundPartial(n, m).copyArrays(unweightedgr.boundPartial(n, m));
            if (!Fourier::sineFT(unweightedsq.boundPartial(n, m), 4.0 * PI * rho, qMin, qDelta, qMax, windowFunction,
                                 broadening))
                return false;

            // Unbound partial
            unweightedsq.unboundPartial(n, m).copyArrays(unweightedgr.unboundPartial(n, m));
            unweightedsq.unboundPartial(n, m).values() -= 1.0;
            if (!Fourier::sineFT(unweightedsq.unboundPartial(n, m), 4.0 * PI * rho, qMin, qDelta, qMax, windowFunction,
                                 broadening))
                return false;
        }
    }

    // Sum into total
    unweightedsq.formTotal(true);

    timer.stop();
    Messenger::print("Finished Fourier transform and summation of partial g(r) into partial S(Q) ({} elapsed, {} comms).\n",
                     timer.totalTimeString(), procPool.accumulatedTimeString());

    return true;
}

// Sum unweighted S(Q) over the supplied Module's target Configurations
bool SQModule::sumUnweightedSQ(ProcessPool &procPool, Module *parentModule, const SQModule *sqModule, GenericList &moduleData,
                               PartialSet &summedUnweightedSQ)
{
    // Create an AtomTypeList containing all AtomTypes present in all target configurations
    AtomTypeList combinedAtomTypes;
    for (Configuration *cfg : parentModule->targetConfigurations())
        combinedAtomTypes.add(cfg->usedAtomTypesList());
    combinedAtomTypes.finalise();

    // Set up PartialSet container
    summedUnweightedSQ.setUpPartials(combinedAtomTypes, parentModule->uniqueName(), "unweighted", "sq", "Q, 1/Angstroms");
    summedUnweightedSQ.setObjectTags(fmt::format("{}//UnweightedSQ", parentModule->uniqueName()));

    // Loop over Configurations again, summing into the PartialSet we have just set up
    // We will keep a running total of the weights associated with each Configuration, and re-weight the entire set of
    // partials at the end.
    double totalWeight = 0.0;
    std::string fingerprint;
    for (Configuration *cfg : parentModule->targetConfigurations())
    {
        // Update fingerprint
        fingerprint +=
            fingerprint.empty() ? fmt::format("{}", cfg->contentsVersion()) : fmt::format("_{}", cfg->contentsVersion());

        // Get weighting factor for this Configuration to contribute to the summed partials
        auto weight = GenericListHelper<double>::value(moduleData, fmt::format("ConfigurationWeight_{}", cfg->niceName()),
                                                       parentModule->uniqueName(), 1.0);
        totalWeight += weight;
        Messenger::print("Weight for Configuration '{}' is {} (total weight is now {}).\n", cfg->name(), weight, totalWeight);

        // Grab partials for Configuration and add into our set
        if (!cfg->moduleData().contains("UnweightedSQ", sqModule->uniqueName()))
            return Messenger::error("Couldn't find UnweightedSQ data for Configuration '{}'.\n", cfg->name());
        auto cfgPartialSQ = GenericListHelper<PartialSet>::value(cfg->moduleData(), "UnweightedSQ", sqModule->uniqueName());
        summedUnweightedSQ.addPartials(cfgPartialSQ, weight);
    }
    summedUnweightedSQ.setFingerprint(fingerprint);

    // Now must normalise our partials to the overall weight of the source configurations
    // TODO CHECK Is this correct?
    summedUnweightedSQ *= 1.0 / totalWeight;

    return true;
}
