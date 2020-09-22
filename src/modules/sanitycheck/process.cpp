// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "main/dissolve.h"
#include "modules/sanitycheck/sanitycheck.h"

// Run main processing
bool SanityCheckModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * This is a parallel routine.
     * As much data as possible is checked over all processes, checking that the "everybody knows everything" ideal is true.
     */

    // Basic checks on data from Dissolve
    auto i = 0;
    for (auto at1 = dissolve.atomTypes().begin(); at1 != dissolve.atomTypes().end(); ++at1, ++i)
    {
        auto j = i;
        for (auto at2 = at1; at2 != dissolve.atomTypes().end(); ++at2, ++j)
        {
            PairPotential *pp = dissolve.pairPotential(*at1, *at2);
            if (!pp)
            {
                Messenger::error("Failed to find PairPotential for AtomTypes '{}' and '{}'.\n", (*at1)->name(), (*at2)->name());
                return false;
            }

            // Check for equality
            Messenger::printVerbose("Sanity checking PairPotential {}-{}...\n", (*at1)->name(), (*at2)->name());
            if (!pp->uOriginal().equality(procPool))
                return Messenger::error("Sanity check failed - PairPotential {}-{} uOriginal are not equal.\n", (*at1)->name(),
                                        (*at2)->name());
            if (!pp->uFull().equality(procPool))
                return Messenger::error("Sanity check failed - PairPotential {}-{} uFull are not equal.\n", (*at1)->name(),
                                        (*at2)->name());
            if (!pp->uAdditional().equality(procPool))
                return Messenger::error("Sanity check failed - PairPotential {}-{} uAdditional are not equal.\n",
                                        (*at1)->name(), (*at2)->name());
        }
    }

    // Loop over all Configurations
    ListIterator<Configuration> configIterator(dissolve.configurations());
    while (Configuration *cfg = configIterator.iterate())
    {
        // TODO This is the most basic sanity checking we can do. Need to extend to bonds, angles, molecules etc.
        // Number of Atoms and atomic positions
        Messenger::printVerbose("Sanity checking Configuration {} atoms...\n", cfg->name());
        if (!procPool.equality(cfg->nAtoms()))
            return Messenger::error("Failed sanity check for Configuration '{}' nAtoms ({}).\n", cfg->name(), cfg->nAtoms());
        for (int n = 0; n < cfg->nAtoms(); ++n)
        {
            auto r = cfg->atom(n)->r();
            if (!procPool.equality(cfg->atom(n)->r()))
                return Messenger::error("Failed sanity check for Configuration '{}' atom position {} ({} {} {}).\n",
                                        cfg->name(), n, r.x, r.y, r.z);
        }

        // Module data within the Configuration
        Messenger::printVerbose("Sanity checking Configuration {} module data...\n", cfg->name());
        if (!cfg->moduleData().equality(procPool))
            return Messenger::error("Failed sanity check for Configuration '{}' module data.\n", cfg->name());
    }

    // Processing module data
    Messenger::printVerbose("Sanity checking processing module data...\n");
    if (!dissolve.processingModuleData().equality(procPool))
        return Messenger::error("Failed sanity check for processing module data.\n");

    Messenger::print("All checked data passed equality tests.\n");

    return true;
}
