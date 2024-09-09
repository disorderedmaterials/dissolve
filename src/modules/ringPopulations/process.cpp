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

    return ExecutionResult::Success;
}
