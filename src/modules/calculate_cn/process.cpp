// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "main/dissolve.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_rdf/rdf.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/integrate1d.h"
#include "procedure/nodes/operatesitepopulationnormalise.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"
#include "procedure/nodes/sum1d.h"

// Run main processing
bool CalculateCNModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for valid CalculateRDF pointer
    if (!sourceRDF_)
        return Messenger::error("No suitable CalculateRDF target set for CalculateCN.\n");

    // Set the target Collect1D and normalisation nodes in the Process1D
    process1D_->keywords().set("SourceData", sourceRDF_->collectDistanceNode());
    siteNormaliser_->keywords().set("Site", std::vector<std::shared_ptr<const SelectProcedureNode>>{sourceRDF_->selectANode()});

    // Execute the analysis on the Configuration targeted by the RDF module
    auto *cfg = sourceRDF_->keywords().get<Configuration *>("Configuration");
    if (!analyser_.execute(procPool, cfg, fmt::format("{}//Analyser", uniqueName()), dissolve.processingModuleData()))
        return Messenger::error("CalculateCN experienced problems with its analysis.\n");

    // Test?
    if (testRangeA_)
    {
        const auto delta = testRangeA_.value() - sum1D_->sum(0);

        Messenger::print("Reference coordination number delta with correct value for range A is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return false;
    }
    if (testRangeB_)
    {
        // Is range B enabled?
        if (!isRangeBEnabled())
            return Messenger::error("Test coordination number for range B supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = testRangeB_.value() - sum1D_->sum(1);

        Messenger::print("Reference coordination number delta with correct value for range B is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return false;
    }
    if (testRangeC_)
    {
        // Is range B enabled?
        if (!isRangeBEnabled())
            return Messenger::error("Test coordination number for range C supplied, but calculation for that range "
                                    "is not active.\n");

        const auto delta = testRangeC_.value() - sum1D_->sum(2);

        Messenger::print("Reference coordination number delta with correct value for range C is {:15.9e} and is {} "
                         "(threshold is {:10.3e})\n",
                         delta, fabs(delta) < testThreshold_ ? "OK" : "NOT OK", testThreshold_);
        if (!procPool.allTrue(fabs(delta) < testThreshold_))
            return false;
    }

    return true;
}
