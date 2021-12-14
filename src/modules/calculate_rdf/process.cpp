// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run set-up stage
bool CalculateRDFModule::setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals) { return true; }

// Run main processing
bool CalculateRDFModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (!targetConfiguration_)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    collectDistance_->keywords().set("RangeX", distanceRange_);
    if (excludeSameMolecule_)
        selectB_->setSameMoleculeExclusions({selectA_});
    else
        selectB_->setSameMoleculeExclusions({});

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(targetConfiguration_->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, targetConfiguration_, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateRDF experienced problems with its analysis.\n");

    return true;
}
