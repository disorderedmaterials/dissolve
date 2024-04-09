// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "math/filters.h"
#include "module/context.h"
#include "modules/bragg/bragg.h"
#include "modules/gr/gr.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

// Set target data
void SQModule::setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                          const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap)
{
    auto sqIt = moduleMap.find(ModuleTypes::GR);
    if (sqIt != moduleMap.end())
        sourceGR_ = dynamic_cast<const GRModule *>(sqIt->second.front());
}

// Run main processing
Module::ExecutionResult SQModule::process(ModuleContext &moduleContext)
{
    /*
     * Calculate S(Q) from Configuration's g(r).
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     */

    if (!sourceGR_)
    {
        Messenger::error("A source GR module must be provided.\n");
        return ExecutionResult::Failed;
    }

    // Print argument/parameter summary
    Messenger::print("SQ: Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin_,
                     qMax_, qDelta_);
    Messenger::print("SQ: Source G(r) will be taken from module '{}'.\n", sourceGR_->name());
    if (windowFunction_ == WindowFunction::Form::None)
        Messenger::print("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        Messenger::print("SQ: Window function to be applied in Fourier transforms is {}.",
                         WindowFunction::forms().keyword(windowFunction_));
    if (averagingLength_)
        Messenger::print("SQ: Partials will be averaged over {} sets (scheme = {}).\n", averagingLength_.value(),
                         Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        Messenger::print("SQ: No averaging of partials will be performed.\n");
    if (qBroadening_.form() == Functions1D::Form::None)
        Messenger::print("SQ: No broadening will be applied to calculated S(Q).");
    else
        Messenger::print("SQ: Broadening to be applied in calculated S(Q) is {} ({}).",
                         Functions1D::function1D().keyword(qBroadening_.form()), qBroadening_.parameterSummary());
    if (sourceBragg_)
    {
        Messenger::print("SQ: Bragg scattering from module '{}' will be included.\n", sourceBragg_->name());
        if (braggQBroadening_.form() == Functions1D::Form::None)
            Messenger::print("SQ: No additional broadening will be applied to calculated Bragg S(Q).");
        else
            Messenger::print("SQ: Broadening to be applied in calculated Bragg S(Q) is {} ({}).",
                             Functions1D::function1D().keyword(braggQBroadening_.form()), braggQBroadening_.parameterSummary());
    }
    Messenger::print("SQ: Save data is {}.\n", DissolveSys::onOff(save_));
    Messenger::print("\n");

    /*
     * Transform target UnweightedGR into the UnweightedSQ.
     */

    // Get unweighted g(r) from the source RDF module
    if (!moduleContext.dissolve().processingModuleData().contains("UnweightedGR", sourceGR_->name()))
    {
        Messenger::error("Couldn't locate source UnweightedGR from module '{}'.\n", sourceGR_->name());
        return ExecutionResult::Failed;
    }
    const auto &unweightedgr =
        moduleContext.dissolve().processingModuleData().value<PartialSet>("UnweightedGR", sourceGR_->name());

    // Get effective atomic density of underlying g(r)
    const auto rho = sourceGR_->effectiveDensity();

    // Does a PartialSet already exist for this Configuration?
    auto uSQObject = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>("UnweightedSQ", name_,
                                                                                           GenericItem::InRestartFileFlag);
    auto &unweightedsq = uSQObject.first;
    if (uSQObject.second == GenericItem::ItemStatus::Created)
        unweightedsq.setUpPartials(unweightedgr.atomTypeMix());

    // Is the PartialSet already up-to-date?
    if (DissolveSys::sameString(
            unweightedsq.fingerprint(),
            fmt::format("{}/{}", moduleContext.dissolve().processingModuleData().version("UnweightedGR", sourceGR_->name()),
                        sourceBragg_
                            ? moduleContext.dissolve().processingModuleData().version("Reflections", sourceBragg_->name())
                            : -1)))
    {
        Messenger::print("SQ: Unweighted partial S(Q) are up-to-date.\n");
        return ExecutionResult::NotExecuted;
    }

    // Transform g(r) into S(Q)
    if (!calculateUnweightedSQ(moduleContext.processPool(), unweightedgr, unweightedsq, qMin_, qDelta_, qMax_, *rho,
                               WindowFunction(windowFunction_), qBroadening_))
        return ExecutionResult::Failed;

    // Include Bragg scattering?
    if (sourceBragg_)
    {
        // Check if reflection data is present
        if (!moduleContext.dissolve().processingModuleData().contains("Reflections", sourceBragg_->name()))
        {
            Messenger::error("Bragg scattering requested to be included, but reflections from the module '{}' "
                             "could not be located.\n",
                             sourceBragg_->name());
            return ExecutionResult::Failed;
        }

        const auto &braggReflections = moduleContext.dissolve().processingModuleData().value<std::vector<BraggReflection>>(
            "Reflections", sourceBragg_->name());
        const auto nReflections = braggReflections.size();
        const auto braggQMax = braggReflections.at(nReflections - 1).q();
        Messenger::print("Found reflections data for module '{}' (nReflections = {}, Q(last) = {} "
                         "Angstroms**-1).\n",
                         sourceBragg_->name(), nReflections, braggQMax);
        const auto &braggAtomTypes =
            moduleContext.dissolve().processingModuleData().value<AtomTypeMix>("SummedAtomTypes", sourceBragg_->name());
        const auto &v0 = moduleContext.dissolve().processingModuleData().value<double>("V0", sourceBragg_->name());

        // Prepare a temporary object for the Bragg partials
        Array2D<Data1D> braggPartials;
        braggPartials.initialise(unweightedsq.nAtomTypes(), unweightedsq.nAtomTypes(), true);
        for (auto &partial : braggPartials)
            partial.initialise(unweightedsq.partial(0, 0));

        // For each partial in our S(Q) array, calculate the broadened Bragg function and blend it
        auto success = for_each_pair_early(
            unweightedsq.atomTypeMix().begin(), unweightedsq.atomTypeMix().end(),
            [&](auto i, auto &at1, auto j, auto &at2) -> EarlyReturn<bool>
            {
                // Locate the corresponding Bragg intensities for this atom type pair
                auto pairIndex = braggAtomTypes.indexOf(at1.atomType(), at2.atomType());
                if (pairIndex.first == -1 || pairIndex.second == -1)
                {
                    Messenger::error(
                        "SQ data has a partial between {} and {}, but no such intensities exist in the reflection data.\n",
                        at1.atomTypeName(), at2.atomTypeName());
                    return false;
                }

                // Grab relevant partial and oop over reflections
                auto &partial = braggPartials[pairIndex];
                for (const auto &reflxn : braggReflections)
                {
                    const auto intensity = reflxn.intensity(pairIndex.first, pairIndex.second);
                    for (auto &&[q, by] : zip(partial.xAxis(), partial.values()))
                        by += braggQBroadening_.y(q - reflxn.q(), q) * intensity * braggQBroadening_.normalisation(q) /
                              (reflxn.q() * q);
                }

                return EarlyReturn<bool>::Continue;
            });
        if (success && !success.value())
            return ExecutionResult::Failed;

        // Finalise partials
        for (auto &partial : braggPartials)
            std::transform(partial.values().begin(), partial.values().end(), partial.values().begin(),
                           [v0](auto &val) { return val * 2.0 * pow(M_PI, 2) / v0; });

        // Remove self-scattering level from partials between the same atom type and remove normalisation from atomic fractions
        dissolve::for_each_pair(ParallelPolicies::par, unweightedsq.atomTypeMix().begin(), unweightedsq.atomTypeMix().end(),
                                [&braggPartials](auto i, auto &atd1, auto j, auto &atd2)
                                {
                                    // Subtract self-scattering level if types are equivalent
                                    if (i == j)
                                        braggPartials[{i, j}] -= atd1.fraction();

                                    // Remove atomic fraction normalisation
                                    braggPartials[{i, j}] /= atd1.fraction() * atd2.fraction();
                                });

        // Blend the bound/unbound and Bragg partials at the higher Q limit
        dissolve::for_each_pair(ParallelPolicies::par, 0, unweightedsq.nAtomTypes(),
                                [&](const int i, const int j)
                                {
                                    // Note: Intramolecular broadening will not be applied to bound terms within the
                                    // calculated Bragg scattering
                                    auto &bound = unweightedsq.boundPartial(i, j);
                                    auto &unbound = unweightedsq.unboundPartial(i, j);
                                    auto &partial = unweightedsq.partial(i, j);
                                    auto &bragg = braggPartials[{i, j}];

                                    for (auto n = 0; n < bound.nValues(); ++n)
                                    {
                                        const auto q = bound.xAxis(n);
                                        if (q <= braggQMax)
                                        {
                                            bound.value(n) = 0.0;
                                            unbound.value(n) = bragg.value(n);
                                            partial.value(n) = bragg.value(n);
                                        }
                                    }
                                });

        // Re-form the total function
        unweightedsq.formTotals(true);
    }

    // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
    if (averagingLength_)
    {
        // Store the current fingerprint, since we must ensure we retain it in the averaged data.
        std::string currentFingerprint{unweightedsq.fingerprint()};

        Averaging::average<PartialSet>(moduleContext.dissolve().processingModuleData(), "UnweightedSQ", name_,
                                       averagingLength_.value(), averagingScheme_);

        // Re-set the object names and fingerprints of the partials
        unweightedsq.setFingerprint(currentFingerprint);
    }

    // Set fingerprint
    unweightedsq.setFingerprint(fmt::format(
        "{}/{}", moduleContext.dissolve().processingModuleData().version("UnweightedGR", sourceGR_->name()),
        sourceBragg_ ? moduleContext.dissolve().processingModuleData().version("Reflections", sourceBragg_->name()) : -1));

    // Save data if requested
    if (save_ && !MPIRunMaster(moduleContext.processPool(), unweightedsq.save(name_, "UnweightedSQ", "sq", "Q, 1/Angstroms")))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
