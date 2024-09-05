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

    // Select all potential bridging oxygen sites - we will determine which actually are
    // involved in NF-BO-NF interactions once we have the available NF sites
    SiteSelector allOxygenSites(targetConfiguration_, bridgingOxygenSpeciesSites_);

    // Select all NF centres
    SiteSelector NF(targetConfiguration_, networkFormerSpeciesSites_);

    return ExecutionResult::Success;
}
