// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/integrate1d.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process1d.h"

// Run main processing
bool CalculateCNModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for valid CalculateRDF pointer
    auto found = false;
    const auto *rdfModule = keywords_.retrieve<const CalculateRDFModule *>("SourceRDF", nullptr, &found);
    if ((!found) || (!rdfModule))
        return Messenger::error("No suitable CalculateRDF target set for CalculateCN.\n");

    // Set the target Collect1D and normalisation nodes in the Process1D
    process1D_->setKeyword<const Collect1DProcedureNode *>("SourceData", rdfModule->collectDistanceNode());
    RefList<const SelectProcedureNode> siteNodes(rdfModule->selectANode());
    siteNormaliser_->setKeyword<RefList<const SelectProcedureNode> &>("Site", siteNodes);

    // Execute the analysis on the Configurations targeted by the RDF module
    for (Configuration *cfg : rdfModule->targetConfigurations())
    {
        if (!analyser_.execute(procPool, cfg, fmt::format("{}//Analyser", uniqueName()), dissolve.processingModuleData()))
            return Messenger::error("CalculateCN experienced problems with its analysis.\n");
    }

    // Test?
    const auto testThreshold = keywords_.asDouble("TestThreshold");
    if (keywords_.isSet("TestRangeA"))
    {
        const auto delta = keywords_.asDouble("TestRangeA") - coordinationNumber(0).value();

        Messenger::print("Reference coordination number delta with correct value for range A is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
        if (!procPool.allTrue(fabs(delta) < testThreshold))
            return false;
    }
    if (keywords_.isSet("TestRangeB"))
    {
        // Is range B enabled?
        if (!isRangeBEnabled())
            return Messenger::error("Test coordination number for range B supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = keywords_.asDouble("TestRangeB") - coordinationNumber(1).value();

        Messenger::print("Reference coordination number delta with correct value for range B is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
        if (!procPool.allTrue(fabs(delta) < testThreshold))
            return false;
    }
    if (keywords_.isSet("TestRangeC"))
    {
        // Is range B enabled?
        if (!isRangeBEnabled())
            return Messenger::error("Test coordination number for range C supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = keywords_.asDouble("TestRangeC") - coordinationNumber(2).value();

        Messenger::print("Reference coordination number delta with correct value for range C is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold ? "OK" : "NOT OK", testThreshold);
        if (!procPool.allTrue(fabs(delta) < testThreshold))
            return false;
    }

    return true;
}
