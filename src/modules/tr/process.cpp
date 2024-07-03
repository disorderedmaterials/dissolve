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

    // Get dependent modules
    if (!sourceNeutronSQ_)
    {
        Messenger::error("A source NewutronSQ module must be provided.\n");
        return ExecutionResult::Failed;
    }

    auto optSQModule = sourceNeutronSQ_->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
    const SQModule *sqModule{nullptr};
    if (optSQModule)
        sqModule = optSQModule.value();
    if (!sqModule)
    {
        Messenger::error("Target module '{}' doesn't source any S(Q) data, so it can't be used as a target for the TR module.",
                         sourceNeutronSQ_->name());
        return ExecutionResult::Failed;
    }

    auto *grModule = sqModule->sourceGR();
    if (!sqModule)
    {
        Messenger::error("A source GR module (in the target SQ module) must be set.\n");
        return ExecutionResult::Failed;
    }
    const auto unweightedGR = moduleData.value<PartialSet>("UnweightedGR", grModule->name());

    // Get effective atomic density of underlying g(r)
    const auto rho = grModule->effectiveDensity();

    auto [weightedTR, wGRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedTR", name_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedTR.setUpPartials(unweightedGR.atomTypeMix());

    // Retrieve weights
    const auto &weights = moduleData.value<NeutronWeights>("FullWeights", sourceNeutronSQ_->name());

    for (auto typeI = 0; typeI < unweightedGR.nAtomTypes(); ++typeI)
    {
        for (auto typeJ = typeI; typeJ < unweightedGR.nAtomTypes(); ++typeJ)
        {
            double intraWeight = weights.intramolecularWeight(typeI, typeJ);
            auto factor = 4.0 * PI * rho.value() * weights.atomTypes()[typeJ].fraction() *
                          weights.atomTypes()[typeI].fraction() * weights.boundCoherentProduct(typeI, typeJ);

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedTR.boundPartial(typeI, typeJ).copyArrays(unweightedGR.boundPartial(typeI, typeJ));
            for (auto &&[x, y] :
                 zip(weightedTR.boundPartial(typeI, typeJ).xAxis(), weightedTR.boundPartial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
            // Unbound partial (multiplied by the full weight)
            weightedTR.unboundPartial(typeI, typeJ).copyArrays(unweightedGR.unboundPartial(typeI, typeJ));
            for (auto &&[x, y] :
                 zip(weightedTR.unboundPartial(typeI, typeJ).xAxis(), weightedTR.unboundPartial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
            // Full partial, summing bound and unbound terms
            weightedTR.partial(typeI, typeJ).copyArrays(unweightedGR.partial(typeI, typeJ));

            for (auto &&[x, y] : zip(weightedTR.partial(typeI, typeJ).xAxis(), weightedTR.partial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
        }
    }

    // Sum into total
    weightedTR.formTotals(false);

    // Save data if requested
    if (saveTR_ && (!MPIRunMaster(moduleContext.processPool(), weightedTR.save(name_, "WeightedTR", "tr", "Q, 1/Angstroms"))))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
