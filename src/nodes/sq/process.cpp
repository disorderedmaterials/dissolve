// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "math/mathFunc.h"

#include "classes/configuration.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "math/filters.h"
#include "module/context.h"
#include "modules/bragg/bragg.h"
#include "modules/gr/gr.h"
#include "modules/sq/sq.h"
#include "nodes/node.h"
#include "nodes/sq/sq.h"
#include "templates/algorithms.h"

// Run main processing
NodeConstants::ProcessResult SQNode::process()
{
    /*
     * Calculate S(Q) from Configuration's g(r).
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     */
    // Print argument/parameter summary
    message("SQ: Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin_, qMax_,
            qDelta_);
    if (windowFunction_ == WindowFunction::Form::None)
        message("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        message("SQ: Window function to be applied in Fourier transforms is {}.",
                WindowFunction::forms().keyword(windowFunction_));
    if (averagingLength_)
        message("SQ: Partials will be averaged over {} sets (scheme = {}).\n", averagingLength_.value(),
                Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        message("SQ: No averaging of partials will be performed.\n");
    if (qBroadening_.form() == Functions1D::Form::None)
        message("SQ: No broadening will be applied to calculated S(Q).");
    else
        message("SQ: Broadening to be applied in calculated S(Q) is {} ({}).",
                Functions1D::forms().keyword(qBroadening_.form()), qBroadening_.parameterSummary());
    if (nullptr)
    {
        if (braggQBroadening_.form() == Functions1D::Form::None)
            message("SQ: No additional broadening will be applied to calculated Bragg S(Q).");
        else
            message("SQ: Broadening to be applied in calculated Bragg S(Q) is {} ({}).",
                    Functions1D::forms().keyword(braggQBroadening_.form()), braggQBroadening_.parameterSummary());
    }
    message("SQ: Save data is {}.\n", DissolveSys::onOff(save_));
    message("\n");

    /*
     * Transform target UnweightedGR into the UnweightedSQ.
     */

    // Get unweighted g(r) from the source RDF module
    // UNUSED
    if (!dissolve().processingModuleData().contains("UnweightedGR", "UNUSED"))
    {
        error("Couldn't locate source UnweightedGR from module '{}'.\n", "UNUSED");
        return NodeConstants::ProcessResult::Failed;
    }
    const auto &unweightedgr = dissolve().processingModuleData().value<PartialSet>("UnweightedGR", "UNUSED");
    // UNUSED

    // Does a PartialSet already exist for this Configuration?
    // UNUSED
    auto uSQObject =
        dissolve().processingModuleData().realiseIf<PartialSet>("UnweightedSQ", name(), GenericItem::InRestartFileFlag);

    auto &unweightedsq = uSQObject.first;
    if (uSQObject.second == GenericItem::ItemStatus::Created)
        unweightedsq.setUpPartials(unweightedgr.atomTypeMix());
    // UNUSED

    unweightedsq_.setUpPartials(unweightedgr_->atomTypeMix());

    // Is the PartialSet already up-to-date?
    if (DissolveSys::sameString(unweightedsq.fingerprint(),
                                std::format("{}/{}", dissolve().processingModuleData().version("UnweightedGR", "UNUSED"),
                                            nullptr ? dissolve().processingModuleData().version("Reflections", "UNUSED") : -1)))
    {
        message("SQ: Unweighted partial S(Q) are up-to-date.\n");
        return ProcessResult::NotExecuted;
    }

    // Transform g(r) into S(Q)
    if (!calculateUnweightedSQ(moduleContext.processPool(), &unweightedgr_, unweightedsq_, qMin_, qDelta_, qMax_, *rho,
                               WindowFunction(windowFunction_), qBroadening_))
        return NodeConstants::ProcessResult::Failed;

    // Include Bragg scattering?
    if (nullptr)
    {
        // Check if reflection data is present
        if (!dissolve().processingModuleData().contains("Reflections", "UNUSED"))
        {
            error("Bragg scattering requested to be included, but reflections from the module '{}' "
                  "could not be located.\n",
                  "UNUSED");
            return NodeConstants::ProcessResult::Failed;
        }

        const auto &braggReflections =
            dissolve().processingModuleData().value<std::vector<BraggReflection>>("Reflections", "UNUSED");
        const auto nReflections = braggReflections.size();
        const auto braggQMax = braggReflections.at(nReflections - 1).q();
        message("Found reflections data for module '{}' (nReflections = {}, Q(last) = {} "
                "Angstroms**-1).\n",
                "UNUSED", nReflections, braggQMax);
        const auto &braggAtomTypes = dissolve().processingModuleData().value<AtomTypeMix>("SummedAtomTypes", "UNUSED");
        const auto &v0 = dissolve().processingModuleData().value<double>("V0", "UNUSED");

        // Prepare a temporary object for the Bragg partials
        Array2D<Data1D> braggPartials;
        braggPartials.initialise(unweightedsq_.nAtomTypes(), unweightedsq_.nAtomTypes(), true);
        for (auto &partial : braggPartials)
            partial.initialise(unweightedsq.partial(0, 0));

        // For each partial in our S(Q) array, calculate the broadened Bragg function and blend it
        auto success = for_each_pair_early(
            unweightedsq_.atomTypeMix().begin(), unweightedsq_.atomTypeMix().end(),
            [&](auto i, auto &at1, auto j, auto &at2) -> EarlyReturn<bool>
            {
                // Locate the corresponding Bragg intensities for this atom type pair
                auto optPairIndex = braggAtomTypes.indexOf(at1.atomType(), at2.atomType());
                if (!optPairIndex)
                {
                    error("SQ data has a partial between {} and {}, but no such intensities exist in the reflection data.\n",
                          at1.atomTypeName(), at2.atomTypeName());
                    return false;
                }

                // Grab relevant partial and oop over reflections
                auto &partial = braggPartials[*optPairIndex];
                auto &[typeI, typeJ] = *optPairIndex;
                for (const auto &reflxn : braggReflections)
                {
                    const auto intensity = reflxn.intensity(typeI, typeJ);
                    for (auto &&[q, by] : zip(partial.xAxis(), partial.values()))
                        by += braggQBroadening_.y(q - reflxn.q(), q) * intensity * braggQBroadening_.normalisation(q) /
                              (reflxn.q() * q);
                }

                return EarlyReturn<bool>::Continue;
            });
        if (success && !success.value())
            return NodeConstants::ProcessResult::Failed;

        // Finalise partials
        for (auto &partial : braggPartials)
            std::transform(partial.values().begin(), partial.values().end(), partial.values().begin(),
                           [v0](auto &val) { return val * 2.0 * pow(M_PI, 2) / v0; });

        // Remove self-scattering level from partials between the same atom type and remove normalisation from atomic fractions
        dissolve::for_each_pair(ParallelPolicies::par, unweightedsq_.atomTypeMix().begin(), unweightedsq_.atomTypeMix().end(),
                                [&braggPartials](auto i, auto &atd1, auto j, auto &atd2)
                                {
                                    // Subtract self-scattering level if types are equivalent
                                    if (i == j)
                                        braggPartials[{i, j}] -= atd1.fraction();

                                    // Remove atomic fraction normalisation
                                    braggPartials[{i, j}] /= atd1.fraction() * atd2.fraction();
                                });

        // Blend the bound/unbound and Bragg partials at the higher Q limit
        dissolve::for_each_pair(ParallelPolicies::par, 0, unweightedsq_.nAtomTypes(),
                                [&](const int i, const int j)
                                {
                                    // Note: Intramolecular broadening will not be applied to bound terms within the
                                    // calculated Bragg scattering
                                    auto &bound = unweightedsq_.boundPartial(i, j);
                                    auto &unbound = unweightedsq_.unboundPartial(i, j);
                                    auto &partial = unweightedsq_.partial(i, j);
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
        unweightedsq_.formTotals(true);
    }

    // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
    if (averagingLength_)
    {
        // Store the current fingerprint, since we must ensure we retain it in the averaged data.
        std::string currentFingerprint{unweightedsq_.fingerprint()};

        Averaging::average<PartialSet>(dissolve().processingModuleData(), "UnweightedSQ", name_, averagingLength_.value(),
                                       averagingScheme_);

        // Re-set the object names and fingerprints of the partials
        unweightedsq_.setFingerprint(currentFingerprint);
    }

    // Set fingerprint
    unweightedsq_.setFingerprint(
        std::format("{}/{}", dissolve().processingModuleData().version("UnweightedGR", "UNUSED"),
                    nullptr ? dissolve().processingModuleData().version("Reflections", "UNUSED") : -1));

    // Save data if requested
    // UNUSED
    if (save_ && !MPIRunMaster(moduleContext.processPool(), unweightedsq.save(name_, "UnweightedSQ", "sq", "Q, 1/Angstroms")))
        return NodeConstants::ProcessResult::Failed;
    // UNUSED

    return NodeConstants::ProcessResult::Success;
}
