// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/module.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sq/sq.h"
#include "modules/tr/tr.h"

// Run main processing
Module::ExecutionResult TRModule::process(ModuleContext &moduleContext)
{
    auto &moduleData = moduleContext.dissolve().processingModuleData();

    // Get NeutronSQ module
    if (!moduleContext.dissolve().processingModuleData().contains("NeutronSQ", sourceNeutronSQ_->name()))
    {
        Messenger::error("Couldn't locate NeutronSQ data from the NeutronSQModule '{}'.\n", sourceNeutronSQ_->name());
        return ExecutionResult::Failed;
    }
    const auto &neutronSQ =
        moduleContext.dissolve().processingModuleData().value<PartialSet>("NeutronSQ", sourceNeutronSQ_->name());

    // Get dependent modules
    if (!sourceNeutronSQ_)
    {
        Messenger::error("[SETUP {}] A source NewutronSQ module must be provided.\n", name_);
        return ExecutionResult::Failed;
    }

    auto optSQModule = sourceNeutronSQ_->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
    const SQModule *sqModule{nullptr};
    if (optSQModule)
        sqModule = optSQModule.value();
    if (!sqModule)
    {
        Messenger::error(
            "[SETUP {}] Target '{}' doesn't source any S(Q) data, so it can't be used as a target for the EPSR module.", name_,
            sourceNeutronSQ_->name());
        return ExecutionResult::Failed;
    }

    auto *grModule = sqModule->sourceGR();
    if (!sqModule)
    {
        Messenger::error("[SETUP {}] A source GR module (in the SQ module) must be provided.\n", name_);
        return ExecutionResult::Failed;
    }

    // Get effective atomic density of underlying g(r)
    const auto rho = grModule->effectiveDensity();
    auto unweightedgr = moduleData.value<PartialSet>("UnweightedGR", sourceNeutronSQ_->name());

    // Calculate and store weights
    const auto &weights = moduleData.value<NeutronWeights>("FullWeights", sourceNeutronSQ_->name());

    int typeI, typeJ;
    for (typeI = 0; typeI < unweightedtr.nAtomTypes(); ++typeI)
    {
        for (typeJ = typeI; typeJ < unweightedgr.nAtomTypes(); ++typeJ)
        {
            // Full partial, summing bound and unbound terms
            weightedtr.partial(typeI, typeJ).copyArrays(unweightedgr.partial(typeI, typeJ));
            weightedtr.partial(typeI, typeJ) += weightedtr.boundPartial(typeI, typeJ);

            auto cj = weights.atomTypes()[typeJ].fraction();

            for (auto &&[x, y] : zip(weightedtr.partial(typeI, typeJ).xAxis(), weightedtr.partial(typeI, typeJ).values()))
            {
                y *= 4.0 * PI * rho.value() * x * cj;
            }
        }
    }
    // Sum into total
    unweightedtr.formTotals(true);

    return ExecutionResult::Success;
}
