// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sequence.h"

// Run set-up stage
bool CalculateRDFModule::setUp(Dissolve &dissolve, ProcessPool &procPool) { return true; }

// Run main processing
bool CalculateRDFModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Ensure any parameters in our nodes are set correctly
    const auto distanceRange = keywords_.asVec3Double("DistanceRange");
    collectDistance_->setKeyword<Vec3<double>>("RangeX", distanceRange);
    const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");
    RefList<SelectProcedureNode> sameMoleculeExclusions;
    if (excludeSameMolecule)
        sameMoleculeExclusions.append(selectA_);
    selectB_->setKeyword<RefList<SelectProcedureNode> &>("ExcludeSameMolecule", sameMoleculeExclusions);

    // Grab Configuration pointer
    auto *cfg = targetConfigurations_.firstItem();

    // Set up process pool - must do this to ensure we are using all available processes
    procPool.assignProcessesToGroups(cfg->processPool());

    // Execute the analysis
    if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData()))
        return Messenger::error("CalculateRDF experienced problems with its analysis.\n");

    return true;
}
