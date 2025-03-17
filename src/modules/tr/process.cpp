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

    // Get target SQ module
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

    // Get target gr module
    auto *grModule = sqModule->sourceGR();
    if (!sqModule)
    {
        Messenger::error("A source GR module (in the target SQ module) must be set.\n");
        return ExecutionResult::Failed;
    }

    // Retrieve weights, GR and SQ
    const auto &weights = moduleData.value<NeutronWeights>("FullWeights", sourceNeutronSQ_->name());
    // Retrieve GR and SQ
    const auto unweightedGR = moduleData.value<PartialSet>("UnweightedGR", grModule->name());
    auto unweightedSQ = moduleData.value<PartialSet>("UnweightedSQ", sqModule->name());
    auto referenceSQ = moduleData.value<Data1D>("ReferenceData", sourceNeutronSQ_->name());

    // Make weightedGR Partial set
    PartialSet representativeGR;
    representativeGR.setUpPartials(unweightedSQ.atomTypeMix(), false);

    // Get effective atomic density of underlying g(r)
    const auto rho = grModule->effectiveDensity();

    // Create weightedTR PartialSet in rtstart file
    auto [weightedTR, wGRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedTR", name_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedTR.setUpPartials(unweightedGR.atomTypeMix(), false);

    // Get Q-range and window function to use for transformation of reference F(Q) to G(r)
    auto refftQMin = refQMin_.value_or(0.0);
    auto refftQMax = refQMax_.value();
    if (refWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("TR: No window function will be applied in Fourier transform of S(Q) to g(r).");
    else
        Messenger::print("TR: Window function to be applied in Fourier transform of S(Q) is {}.",
                         WindowFunction::forms().keyword(refWindowFunction_));

    // FT Reference data to ReresentativeTotalGR
    Fourier::sineFT(referenceSQ, 1.0 / (2 * PI * PI * rho.value()), refftQMin, qDelta_, refftQMax, refWindowFunction_,
                    refQBroadening_);

    // Get Q-range and window function to use for transformation of total F(Q) to G(r)
    auto repftQMin = repQMin_.value_or(0.0);
    auto repftQMax = repQMax_.value();
    if (repWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("TR: No window function will be applied in Fourier transform of S(Q) to g(r).");
    else
        Messenger::print("TR: Window function to be applied in Fourier transform of S(Q) is {}.",
                         WindowFunction::forms().keyword(repWindowFunction_));
    // FT unweightedSQ to unweightedGR to get better representation of calculations
    dissolve::for_each_pair(
        ParallelPolicies::par, 0, unweightedSQ.nAtomTypes(),
        [&](int n, int m)
        {
            // Total partial
            representativeGR.partial(n, m).copyArrays(unweightedSQ.partial(n, m));
            Fourier::sineFT(representativeGR.partial(n, m), 1.0 / (2 * PI * PI * rho.value()), repftQMin, qDelta_, repftQMax,
                            WindowFunction::Form::None, repQBroadening_);
            representativeGR.partial(n, m) += 1.0;

            // Bound partial
            representativeGR.boundPartial(n, m).copyArrays(unweightedSQ.boundPartial(n, m));
            Fourier::sineFT(representativeGR.boundPartial(n, m), 1.0 / (2 * PI * PI * rho.value()), repftQMin, qDelta_,
                            repftQMax, WindowFunction::Form::None, repQBroadening_);

            // Unbound partial
            representativeGR.unboundPartial(n, m).copyArrays(unweightedSQ.unboundPartial(n, m));
            Fourier::sineFT(representativeGR.unboundPartial(n, m), 1.0 / (2 * PI * PI * rho.value()), repftQMin, qDelta_,
                            repftQMax, WindowFunction::Form::None, repQBroadening_);
            representativeGR.unboundPartial(n, m) += 1.0;
        },
        false);

    // Calculate TR from GR
    auto [referenceCalcTR, bGRstatus] =
        moduleContext.dissolve().processingModuleData().realiseIf<Data1D>("ReferenceTR", name_, GenericItem::InRestartFileFlag);

    referenceCalcTR.copyArrays(referenceSQ);
    // T(r)=4 * PI * x * rho * (G(r) + BCAS)
    // 1) (G(r) + BCAS)
    referenceCalcTR += weights.boundCoherentAverageOfSquares();
    // 2) 4 * PI * x * rho
    referenceCalcTR *= 4 * PI * rho.value();
    referenceCalcTR *= referenceCalcTR.xAxis();

    // Calculate RepresentativeTR
    auto [representativeTR, rTRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "RepresentativeTR", name_, GenericItem::InRestartFileFlag);
    if (rTRstatus == GenericItem::ItemStatus::Created)
        representativeTR.setUpPartials(representativeGR.atomTypeMix(), false);

    dissolve::for_each_pair(
        ParallelPolicies::par, 0, representativeGR.nAtomTypes(),
        [&weights, &rho, &representativeGR, &representativeTR](const auto typeI, const auto typeJ)
        {
            double intraWeight = weights.intramolecularWeight(typeI, typeJ);
            auto cj = weights.atomTypes()[typeJ].fraction();
            auto factor = 4.0 * PI * rho.value() * cj;
            representativeTR.boundPartial(typeI, typeJ).copyArrays(representativeGR.boundPartial(typeI, typeJ));
            representativeTR.unboundPartial(typeI, typeJ).copyArrays(representativeGR.unboundPartial(typeI, typeJ));
            representativeTR.partial(typeI, typeJ).copyArrays(representativeGR.partial(typeI, typeJ));
            representativeTR.boundPartial(typeI, typeJ).copyArrays(representativeGR.boundPartial(typeI, typeJ));
            for (auto &&[x, y] :
                 zip(representativeTR.boundPartial(typeI, typeJ).xAxis(), representativeTR.boundPartial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
            // Unbound partial (multiplied by the full weight)
            representativeTR.unboundPartial(typeI, typeJ).copyArrays(representativeGR.unboundPartial(typeI, typeJ));
            for (auto &&[x, y] : zip(representativeTR.unboundPartial(typeI, typeJ).xAxis(),
                                     representativeTR.unboundPartial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
            // Full partial, summing bound and unbound terms
            representativeTR.partial(typeI, typeJ).copyArrays(representativeGR.partial(typeI, typeJ));
            for (auto &&[x, y] :
                 zip(representativeTR.partial(typeI, typeJ).xAxis(), representativeTR.partial(typeI, typeJ).values()))
            {
                y *= x * factor;
            }
        },
        false);

    // Calculate weightedTR
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

    // Sum into totals
    weightedTR.formTRTotals(weights);
    representativeTR.formTRTotals(weights);

    // Save data if requested
    if (saveTR_ && (!MPIRunMaster(moduleContext.processPool(), weightedTR.save(name_, "WeightedTR", "tr", "Q, 1/Angstroms"))))
        return ExecutionResult::Failed;
    // Save data if requested
    if (saveRepTR_ &&
        (!MPIRunMaster(moduleContext.processPool(), representativeTR.save(name_, "RepresentativeTR", "tr", "Q, 1/Angstroms"))))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
