// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "keywords/module.h"
#include "main/dissolve.h"
#include "math/ft.h"
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

    // Retrieve weights
    const auto &weights = moduleData.value<NeutronWeights>("FullWeights", sourceNeutronSQ_->name());
    // Retrieve GR and SQ
    const auto unweightedGR = moduleData.value<PartialSet>("UnweightedGR", grModule->name());
    auto referenceSQ = moduleData.value<Data1D>("ReferenceData", sourceNeutronSQ_->name());
    PartialSet weightedGR;
    weightedGR.setUpPartials(referenceSQ.atomTypeMix(), false);

    // Get effective atomic density of underlying g(r)
    const auto rho = grModule->effectiveDensity();

    auto [weightedTR, wGRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedTR", name_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedTR.setUpPartials(unweightedGR.atomTypeMix(), false);
    // Get Q-range and window function to use for transformation of F(Q) to G(r)
    auto ftQMin = qMin_.value_or(0.0);
    auto ftQMax = qMax_.value();
    if (windowFunction_ == WindowFunction::Form::None)
        Messenger::print("[SETUP {}] No window function will be applied in Fourier transform of S(Q) to g(r).", name_);
    else
        Messenger::print("[SETUP {}] Window function to be applied in Fourier transform of S(Q) is {}.", name_,
                         WindowFunction::forms().keyword(windowFunction_));
    dissolve::for_each_pair(
        ParallelPolicies::par, 0, referenceSQ.nAtomTypes(),
        [&](int n, int m)
        {
            // Total partial
            weightedGR.partial(n, m).copyArrays(unweightedSQ.partial(n, m));
            Fourier::sineFT(weightedGR.partial(n, m), 1.0 / (2 * PI * PI * rho.value()), ftQMin, qDelta_, ftQMax,
                            windowFunction_, qBroadening_);
            weightedGR.partial(n, m) += 1.0;

            // Bound partial
            weightedGR.boundPartial(n, m).copyArrays(unweightedSQ.boundPartial(n, m));
            Fourier::sineFT(weightedGR.boundPartial(n, m), 1.0 / (2 * PI * PI * rho.value()), ftQMin, qDelta_, ftQMax,
                            windowFunction_, qBroadening_);

            // Unbound partial
            weightedGR.unboundPartial(n, m).copyArrays(unweightedSQ.unboundPartial(n, m));
            Fourier::sineFT(weightedGR.unboundPartial(n, m), 1.0 / (2 * PI * PI * rho.value()), ftQMin, qDelta_, ftQMax,
                            windowFunction_, qBroadening_);
            weightedGR.unboundPartial(n, m) += 1.0;
        },
        false);

    auto [broadenedTR, bGRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "BroadenedTR", name_, GenericItem::InRestartFileFlag);
    if (bGRstatus == GenericItem::ItemStatus::Created)
        broadenedTR.setUpPartials(weightedGR.atomTypeMix(), false);

    dissolve::for_each_pair(
        ParallelPolicies::par, 0, weightedGR.nAtomTypes(),
        [&](const auto typeI, const auto typeJ)
        {
            double intraWeight = weights.intramolecularWeight(typeI, typeJ);
            auto cj = weights.atomTypes()[typeJ].fraction();
            auto factor = 4.0 * PI * rho.value() * cj;

            broadenedTR.boundPartial(typeI, typeJ).copyArrays(weightedGR.boundPartial(typeI, typeJ));
            broadenedTR.unboundPartial(typeI, typeJ).copyArrays(weightedGR.unboundPartial(typeI, typeJ));
            broadenedTR.partial(typeI, typeJ).copyArrays(weightedGR.partial(typeI, typeJ));

            broadenedTR.boundPartial(typeI, typeJ).copyArrays(weightedGR.boundPartial(typeI, typeJ));
            for (auto &&[x, y] :
                 zip(broadenedTR.boundPartial(typeI, typeJ).xAxis(), broadenedTR.boundPartial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
            // Unbound partial (multiplied by the full weight)
            broadenedTR.unboundPartial(typeI, typeJ).copyArrays(weightedGR.unboundPartial(typeI, typeJ));
            for (auto &&[x, y] :
                 zip(broadenedTR.unboundPartial(typeI, typeJ).xAxis(), broadenedTR.unboundPartial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
            // Full partial, summing bound and unbound terms
            broadenedTR.partial(typeI, typeJ).copyArrays(weightedGR.partial(typeI, typeJ));

            for (auto &&[x, y] : zip(broadenedTR.partial(typeI, typeJ).xAxis(), broadenedTR.partial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
        },
        false);

    dissolve::for_each_pair(
        ParallelPolicies::seq, 0, unweightedGR.nAtomTypes(),
        [&weights, &rho, &unweightedGR, &weightedTR](const auto typeI, const auto typeJ)
        {
            double intraWeight = weights.intramolecularWeight(typeI, typeJ);
            auto cj = weights.atomTypes()[typeJ].fraction();
            auto factor = 4.0 * PI * rho.value() * cj;

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
        },
        false);

    // Sum into total
    weightedTR.formTRTotals(weights);
    broadenedTR.formTRTotals(weights);
    // broadenedTR.formTRTotals(weights);
    //  Save data if requested
    if (saveTR_ && (!MPIRunMaster(moduleContext.processPool(), weightedTR.save(name_, "WeightedTR", "tr", "Q, 1/Angstroms"))))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
