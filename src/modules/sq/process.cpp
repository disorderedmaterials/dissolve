// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "math/filters.h"
#include "modules/bragg/bragg.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"

// Run main processing
bool SQModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate S(Q) from Configuration's g(r).
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     */

    const RDFModule *rdfModule = keywords_.retrieve<const RDFModule *>("SourceRDFs", nullptr);
    if (!rdfModule)
        return Messenger::error("A source RDF module must be provided.\n");
    const auto averaging = keywords_.asInt("Averaging");
    const auto includeBragg = keywords_.asBool("IncludeBragg");
    const auto &braggQBroadening = keywords_.retrieve<BroadeningFunction>("BraggQBroadening", BroadeningFunction());
    const auto averagingScheme = keywords_.enumeration<Averaging::AveragingScheme>("AveragingScheme");
    const auto &qBroadening = keywords_.retrieve<BroadeningFunction>("QBroadening", BroadeningFunction());
    const auto qDelta = keywords_.asDouble("QDelta");
    const auto qMin = keywords_.asDouble("QMin");
    const auto qMax = keywords_.asDouble("QMax");
    const auto saveData = keywords_.asBool("Save");
    const auto &windowFunction = keywords_.retrieve<WindowFunction>("WindowFunction", WindowFunction());

    // Print argument/parameter summary
    Messenger::print("SQ: Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin,
                     qMax, qDelta);
    Messenger::print("SQ: Source RDFs will be taken from module '{}'.\n", rdfModule->uniqueName());
    if (windowFunction.function() == WindowFunction::NoWindow)
        Messenger::print("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        Messenger::print("SQ: Window function to be applied in Fourier transforms is {} ({}).",
                         WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary());
    if (averaging <= 1)
        Messenger::print("SQ: No averaging of partials will be performed.\n");
    else
        Messenger::print("SQ: Partials will be averaged over {} sets (scheme = {}).\n", averaging,
                         Averaging::averagingSchemes().keyword(averagingScheme));
    if (qBroadening.function() == BroadeningFunction::NoFunction)
        Messenger::print("SQ: No broadening will be applied to calculated S(Q).");
    else
        Messenger::print("SQ: Broadening to be applied in calculated S(Q) is {} ({}).",
                         BroadeningFunction::functionType(qBroadening.function()), qBroadening.parameterSummary());
    if (includeBragg)
    {
        Messenger::print("SQ: Bragg scattering will be calculated from reflection data in target "
                         "Configurations, if present.\n");
        if (braggQBroadening.function() == BroadeningFunction::NoFunction)
            Messenger::print("SQ: No additional broadening will be applied to calculated Bragg S(Q).");
        else
            Messenger::print("SQ: Additional broadening to be applied in calculated Bragg S(Q) is {} ({}).",
                             BroadeningFunction::functionType(braggQBroadening.function()),
                             braggQBroadening.parameterSummary());
    }
    Messenger::print("SQ: Save data is {}.\n", DissolveSys::onOff(saveData));
    Messenger::print("\n");

    /*
     * Transform target UnweightedGR into the UnweightedSQ.
     */

    // Get unweighted g(r) from the source RDF module
    if (!dissolve.processingModuleData().contains("UnweightedGR", rdfModule->uniqueName()))
        return Messenger::error("Couldn't locate source UnweightedGR from module '{}'.\n", rdfModule->uniqueName());
    const auto &unweightedgr =
        GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedGR", rdfModule->uniqueName());

    // Get effective atomic density of underlying g(r)
    const auto rho = rdfModule->effectiveDensity();

    // Does a PartialSet already exist for this Configuration?
    bool wasCreated;
    auto &unweightedsq = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "UnweightedSQ", uniqueName_,
                                                                GenericItem::InRestartFileFlag, &wasCreated);
    if (wasCreated)
        unweightedsq.setUpPartials(unweightedgr.atomTypes(), uniqueName_, "unweighted", "sq", "Q, 1/Angstroms");

    // Is the PartialSet already up-to-date?
    if (DissolveSys::sameString(unweightedsq.fingerprint(),
                                fmt::format("{}/{}",
                                            dissolve.processingModuleData().version("UnweightedGR", rdfModule->uniqueName()),
                                            includeBragg ? dissolve.processingModuleData().version("BraggReflections") : -1)))
    {
        Messenger::print("SQ: Unweighted partial S(Q) are up-to-date.\n");
        return true;
    }

    // Transform g(r) into S(Q)
    if (!calculateUnweightedSQ(procPool, unweightedgr, unweightedsq, qMin, qDelta, qMax, rho, windowFunction, qBroadening))
        return false;

    // Include Bragg scattering?
    if (includeBragg)
    {
        // Get associated BraggModule
        const auto *braggModule = keywords_.retrieve<const BraggModule *>("SourceReflections", nullptr);
        if (!braggModule)
            return Messenger::error("A source Bragg module must be provided.\n");

        // Check if reflection data is present
        if (!dissolve.processingModuleData().contains("BraggReflections", braggModule->uniqueName()))
            return Messenger::error("Bragg scattering requested to be included, but reflections from the module '{}' "
                                    "could not be located.\n",
                                    braggModule->uniqueName());
        const auto &braggReflections = GenericListHelper<Array<BraggReflection>>::value(
            dissolve.processingModuleData(), "BraggReflections", braggModule->uniqueName(), Array<BraggReflection>());
        const auto nReflections = braggReflections.nItems();
        const auto braggQMax = braggReflections.at(nReflections - 1).q();
        Messenger::print("Found BraggReflections data for module '{}' (nReflections = {}, QMax = {} "
                         "Angstroms**-1).\n",
                         braggModule->uniqueName(), nReflections, braggQMax);

        // Create a temporary array into which our broadened Bragg partials will be placed
        bool created;
        auto &braggPartials = GenericListHelper<Array2D<Data1D>>::realise(dissolve.processingModuleData(), "BraggPartials",
                                                                          uniqueName_, GenericItem::NoFlag, &created);
        if (created)
        {
            // Initialise the array
            braggPartials.initialise(unweightedsq.nAtomTypes(), unweightedsq.nAtomTypes(), true);

            for_each_pair(0, unweightedsq.nAtomTypes(), [&](const int i, const int j) {
                braggPartials[{i, j}] = unweightedsq.partial(0, 0);
            });
        }
        for_each_pair(0, unweightedsq.nAtomTypes(), [&](const int i, const int j) {
            std::fill(braggPartials[{i, j}].values().begin(), braggPartials[{i, j}].values().end(), 0.0);
        });

        // First, re-bin the reflection data into the arrays we have just set up
        // TODO Disabled pending #277
        //         if (!BraggModule::reBinReflections(procPool, cfg, braggPartials))
        //             return false;
        //
        //         // Apply necessary broadening
        //         for (auto i = 0; i < unweightedsq.nAtomTypes(); ++i)
        //         {
        //             for (auto j = i; j < unweightedsq.nAtomTypes(); ++j)
        //             {
        //                 // Bragg-specific broadening
        //                 Filters::convolve(braggPartials.at(i, j), braggQBroadening, true);
        //
        //                 // Local 'QBroadening' term
        //                 Filters::convolve(braggPartials.at(i, j), qBroadening, true);
        //             }
        //         }
        //
        //         // Remove self-scattering level from partials between the same atom type and remove normalisation from
        //         // atomic fractions
        //         for (auto i = 0; i < unweightedsq.nAtomTypes(); ++i)
        //         {
        //             for (auto j = i; j < unweightedsq.nAtomTypes(); ++j)
        //             {
        //                 // Subtract self-scattering level if types are equivalent
        //                 if (i == j)
        //                     braggPartials.at(i, i) -= cfg->usedAtomTypeData(i).fraction();
        //
        //                 // Remove atomic fraction normalisation
        //                 braggPartials.at(i, j) /= cfg->usedAtomTypeData(i).fraction() *
        //                 cfg->usedAtomTypeData(j).fraction();
        //             }
        //         }

        // Blend the bound/unbound and Bragg partials at the higher Q limit
        for_each_pair(0, unweightedsq.nAtomTypes(), [&](const int i, const int j) {
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
        unweightedsq.formTotal(true);
    }

    // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
    if (averaging > 1)
    {
        // Store the current fingerprint, since we must ensure we retain it in the averaged data.
        std::string currentFingerprint{unweightedsq.fingerprint()};

        Averaging::average<PartialSet>(dissolve.processingModuleData(), "UnweightedSQ", uniqueName_, averaging,
                                       averagingScheme);

        // Need to rename data within the contributing datasets to avoid clashes with the averaged data
        for (auto n = averaging; n > 0; --n)
        {
            if (!dissolve.processingModuleData().contains(fmt::format("UnweightedSQ_{}", n), uniqueName_))
                continue;
            auto &p = GenericListHelper<PartialSet>::retrieve(dissolve.processingModuleData(),
                                                              fmt::format("UnweightedSQ_{}", n), uniqueName_);
            p.setObjectTags(fmt::format("{}//UnweightedSQ", uniqueName_), fmt::format("Avg{}", n));
        }

        // Re-set the object names and fingerprints of the partials
        unweightedsq.setFingerprint(currentFingerprint);
    }

    // Set names of resources (Data1D) within the PartialSet
    unweightedsq.setObjectTags(fmt::format("{}//{}", uniqueName_, "UnweightedSQ"));
    unweightedsq.setFingerprint(fmt::format("{}/{}",
                                            dissolve.processingModuleData().version("UnweightedGR", rdfModule->uniqueName()),
                                            includeBragg ? dissolve.processingModuleData().version("BraggReflections") : -1));
    // Save data if requested
    if (saveData && !MPIRunMaster(procPool, unweightedsq.save()))
        return false;

    return true;
}
