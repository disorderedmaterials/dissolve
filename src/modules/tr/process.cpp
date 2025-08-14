// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/mathFunc.h"

#include "keywords/module.h"
#include "main/dissolve.h"
#include "math/ft.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sq/sq.h"
#include "modules/tr/tr.h"

#include <ranges>

// Run main processing
Module::ExecutionResult TRModule::process(Dissolve &dissolve)
{
    auto &moduleData = dissolve.processingModuleData();

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
    representativeGR.initialise(unweightedSQ);

    // Get effective atomic density of underlying g(r)
    const auto rho = grModule->effectiveDensity();

    // Create weightedTR PartialSet in rtstart file
    auto [weightedTR, wGRstatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedTR", name_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedTR.initialise(unweightedGR);

    // Get Q-range and window function to use for transformation of reference F(Q) to G(r)
    auto refftQMin = refQMin_.value_or(0.0);
    auto refftQMax = refQMax_.value();
    if (refWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("TR: No window function will be applied in Fourier transform of S(Q) to g(r).");
    else
        Messenger::print("TR: Window function to be applied in Fourier transform of S(Q) is {}.",
                         WindowFunction::forms().keyword(refWindowFunction_));

    // FT Reference data to ReresentativeTotalGR
    Fourier::sineFT(referenceSQ, 1.0 / (2 * M_PI * M_PI * rho.value()), refftQMin, qDelta_, refftQMax, refWindowFunction_,
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
    // Full partials
    dissolve::for_each(ParallelPolicies::par, unweightedSQ.partials().begin(), unweightedSQ.partials().end(),
                       [&](const auto &pair)
                       {
                           auto &gr = representativeGR.partials().map()[pair.first];
                           gr.copyArrays(pair.second);
                           Fourier::sineFT(gr, 1.0 / (2 * M_PI * M_PI * rho.value()), repftQMin, qDelta_, repftQMax,
                                           WindowFunction::Form::None, repQBroadening_);
                           gr += 1.0;
                       });

    // Bound partials
    dissolve::for_each(ParallelPolicies::par, unweightedSQ.boundPartials().begin(), unweightedSQ.boundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &gr = representativeGR.boundPartials().map()[pair.first];
                           gr.copyArrays(pair.second);
                           Fourier::sineFT(gr, 1.0 / (2 * M_PI * M_PI * rho.value()), repftQMin, qDelta_, repftQMax,
                                           WindowFunction::Form::None, repQBroadening_);
                       });

    // Unbound partials
    dissolve::for_each(ParallelPolicies::par, unweightedSQ.unboundPartials().begin(), unweightedSQ.unboundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &gr = representativeGR.unboundPartials().map()[pair.first];
                           gr.copyArrays(pair.second);
                           Fourier::sineFT(gr, 1.0 / (2 * M_PI * M_PI * rho.value()), repftQMin, qDelta_, repftQMax,
                                           WindowFunction::Form::None, repQBroadening_);
                           gr += 1.0;
                       });

    // Calculate TR from GR
    auto [referenceCalcTR, bGRstatus] =
        dissolve.processingModuleData().realiseIf<Data1D>("ReferenceTR", name_, GenericItem::InRestartFileFlag);

    referenceCalcTR.copyArrays(referenceSQ);
    // T(r)=4 * PI * x * rho * (G(r) + BCAS)
    // 1) (G(r) + BCAS)
    referenceCalcTR += weights.boundCoherentAverageOfSquares();
    // 2) 4 * PI * x * rho
    referenceCalcTR *= 4 * M_PI * rho.value();
    referenceCalcTR *= referenceCalcTR.xAxis();

    // Calculate RepresentativeTR
    auto [representativeTR, rTRstatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("RepresentativeTR", name_, GenericItem::InRestartFileFlag);
    if (rTRstatus == GenericItem::ItemStatus::Created)
        representativeTR.initialise(representativeGR);

    auto typeFractions = representativeGR.atomTypeFractions();

    dissolve::for_each_pair(
        ParallelPolicies::par, typeFractions,
        [&weights, &rho, &representativeGR, &representativeTR](const auto indexI, const auto &popI, const auto indexJ,
                                                               const auto &popJ)
        {
            auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};
            double intraWeight = weights.intramolecularWeight(indexI, indexJ);
            auto cj = weights.atomTypes().mix().get(popJ.first)->fraction();

            auto factor = 4.0 * M_PI * rho.value() * cj;
            representativeTR.boundPartials().get(key).copyArrays(representativeGR.boundPartials().get(key));
            representativeTR.unboundPartials().get(key).copyArrays(representativeGR.unboundPartials().get(key));
            representativeTR.partials().get(key).copyArrays(representativeGR.partials().get(key));
            representativeTR.boundPartials().get(key).copyArrays(representativeGR.boundPartials().get(key));

            for (auto &&[x, y] :
                 zip(representativeTR.boundPartials().get(key).xAxis(), representativeTR.boundPartials().get(key).values()))
            {
                y *= x * factor;
            }

            // Unbound partial (multiplied by the full weight)
            representativeTR.unboundPartials().get(key).copyArrays(representativeGR.unboundPartials().get(key));
            for (auto &&[x, y] :
                 zip(representativeTR.unboundPartials().get(key).xAxis(), representativeTR.unboundPartials().get(key).values()))
            {
                y *= x * factor;
            }

            // Full partial, summing bound and unbound terms
            representativeTR.partials().get(key).copyArrays(representativeGR.partials().get(key));
            for (auto &&[x, y] :
                 zip(representativeTR.partials().get(key).xAxis(), representativeTR.partials().get(key).values()))
            {
                y *= x * factor;
            }
        },
        false);

    // Calculate weightedTR
    dissolve::for_each_pair(
        ParallelPolicies::par, typeFractions,
        [&weights, &rho, &unweightedGR, &weightedTR](const auto indexI, const auto &popI, const auto indexJ, const auto &popJ)
        {
            auto key = DoubleKeyedMapKey{popI.first->name(), popJ.first->name()};
            double intraWeight = weights.intramolecularWeight(indexI, indexJ);
            auto cj = weights.atomTypes().mix().get(popJ.first)->fraction();
            auto factor = 4.0 * M_PI * rho.value() * cj;

            // Bound (intramolecular) partial (multiplied by the bound term weight)
            weightedTR.boundPartials().get(key).copyArrays(unweightedGR.boundPartials().get(key));
            for (auto &&[x, y] : zip(weightedTR.boundPartials().get(key).xAxis(), weightedTR.boundPartials().get(key).values()))
            {
                y *= x * factor;
            }

            // Unbound partial (multiplied by the full weight)
            weightedTR.unboundPartials().get(key).copyArrays(unweightedGR.unboundPartials().get(key));
            for (auto &&[x, y] :
                 zip(weightedTR.unboundPartials().get(key).xAxis(), weightedTR.unboundPartials().get(key).values()))
            {
                y *= x * factor;
            }

            // Full partial, summing bound and unbound terms
            weightedTR.partials().get(key).copyArrays(unweightedGR.partials().get(key));
            for (auto &&[x, y] : zip(weightedTR.partials().get(key).xAxis(), weightedTR.partials().get(key).values()))
            {
                y *= x * factor;
            }
        },
        false);

    // Sum into totals
    weightedTR.formTRTotals(weights);
    representativeTR.formTRTotals(weights);

    // Save data if requested
    if (saveTR_ && (!weightedTR.save(name_, "WeightedTR", "tr", "Q, 1/Angstroms")))
        return ExecutionResult::Failed;
    if (saveRepTR_ && (!representativeTR.save(name_, "RepresentativeTR", "tr", "Q, 1/Angstroms")))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
