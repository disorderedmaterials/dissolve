// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "analyser/siteFilter.h"
#include "analyser/siteSelector.h"
#include "main/dissolve.h"
#include "math/integerHistogram1D.h"
#include "math/integrator.h"
#include "module/context.h"
#include "modules/ringPopulations/ringPopulations.h"

// Run main processing
Module::ExecutionResult RingPopulationsModule::process(ModuleContext &moduleContext)
{
    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Select all potential ring forming sites
    SiteSelector allRingFormerSites(targetConfiguration_, a_);

    // Select all bridging sites between rings
    SiteSelector bridingingSites(targetConfiguration_, b_);

    /*
     * Algorithm as suggested in Phys. Rev. B 1991, 44, 4925, section VI
     *	1) Select an arbitrary vertex (atom) z
     *	2) Generate each ring containing z, of max length m, and test if it is an SP ring
     * 	3) Delete z
     * 	4) Repeat 1) - 3) until no vertices remain.
     */

    // Step 1)
    for (auto &z : allRingFormerSites.sites())
    {
        int nOk, ringSize, diameter, scope;
        bool odd;

        // Step 2)
        for (int n = 0; n < maxRingSize_; ++n)
            n + 1;
    }

    return ExecutionResult::Success;
}
