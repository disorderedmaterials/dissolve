// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
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
    const auto *braggModule = keywords_.retrieve<const BraggModule *>("IncludeBragg");
    const auto &braggQBroadening = keywords_.retrieve<Functions::Function1DWrapper>("BraggQBroadening");
    const auto averagingScheme = keywords_.enumeration<Averaging::AveragingScheme>("AveragingScheme");
    const auto &qBroadening = keywords_.retrieve<Functions::Function1DWrapper>("QBroadening");
    const auto qDelta = keywords_.asDouble("QDelta");
    const auto qMin = keywords_.asDouble("QMin");
    const auto qMax = keywords_.asDouble("QMax");
    const auto saveData = keywords_.asBool("Save");
    const auto wf = keywords_.enumeration<WindowFunction::Form>("WindowFunction");

    // Print argument/parameter summary
    Messenger::print("SQ: Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin,
                     qMax, qDelta);
    Messenger::print("SQ: Source RDFs will be taken from module '{}'.\n", rdfModule->uniqueName());
    if (wf == WindowFunction::Form::None)
        Messenger::print("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        Messenger::print("SQ: Window function to be applied in Fourier transforms is {}.", WindowFunction::forms().keyword(wf));
    if (averaging <= 1)
        Messenger::print("SQ: No averaging of partials will be performed.\n");
    else
        Messenger::print("SQ: Partials will be averaged over {} sets (scheme = {}).\n", averaging,
                         Averaging::averagingSchemes().keyword(averagingScheme));
    if (qBroadening.type() == Functions::Function1D::None)
        Messenger::print("SQ: No broadening will be applied to calculated S(Q).");
    else
        Messenger::print("SQ: Broadening to be applied in calculated S(Q) is {} ({}).",
                         Functions::function1D().keyword(qBroadening.type()), qBroadening.parameterSummary());
    if (braggModule)
    {
        Messenger::print("SQ: Bragg scattering from module '{}' will be included.\n", braggModule->uniqueName());
        if (braggQBroadening.type() == Functions::Function1D::None)
            Messenger::print("SQ: No additional broadening will be applied to calculated Bragg S(Q).");
        else
            Messenger::print("SQ: Broadening to be applied in calculated Bragg S(Q) is {} ({}).",
                             Functions::function1D().keyword(braggQBroadening.type()), braggQBroadening.parameterSummary());
    }
    Messenger::print("SQ: Save data is {}.\n", DissolveSys::onOff(saveData));
    Messenger::print("\n");

    /*
     * Transform target UnweightedGR into the UnweightedSQ.
     */

    // Get unweighted g(r) from the source RDF module
    if (!dissolve.processingModuleData().contains("UnweightedGR", rdfModule->uniqueName()))
        return Messenger::error("Couldn't locate source UnweightedGR from module '{}'.\n", rdfModule->uniqueName());
    const auto &unweightedgr = dissolve.processingModuleData().value<PartialSet>("UnweightedGR", rdfModule->uniqueName());

    // Get effective atomic density of underlying g(r)
    const auto rho = rdfModule->effectiveDensity();

    // Does a PartialSet already exist for this Configuration?
    auto uSQObject =
        dissolve.processingModuleData().realiseIf<PartialSet>("UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
    auto &unweightedsq = uSQObject.first;
    if (uSQObject.second == GenericItem::ItemStatus::Created)
        unweightedsq.setUpPartials(unweightedgr.atomTypes());

    // Is the PartialSet already up-to-date?
    if (DissolveSys::sameString(
            unweightedsq.fingerprint(),
            fmt::format("{}/{}", dissolve.processingModuleData().version("UnweightedGR", rdfModule->uniqueName()),
                        braggModule ? dissolve.processingModuleData().version("Reflections", braggModule->uniqueName()) : -1)))
    {
        Messenger::print("SQ: Unweighted partial S(Q) are up-to-date.\n");
        return true;
    }

    // Transform g(r) into S(Q)
    if (!calculateUnweightedSQ(procPool, unweightedgr, unweightedsq, qMin, qDelta, qMax, rho, WindowFunction(wf), qBroadening))
        return false;

    // Include Bragg scattering?
    if (braggModule)
    {
        // Check if reflection data is present
        if (!dissolve.processingModuleData().contains("Reflections", braggModule->uniqueName()))
            return Messenger::error("Bragg scattering requested to be included, but reflections from the module '{}' "
                                    "could not be located.\n",
                                    braggModule->uniqueName());
        const auto &braggReflections =
            dissolve.processingModuleData().value<std::vector<BraggReflection>>("Reflections", braggModule->uniqueName());
        const auto nReflections = braggReflections.size();
        const auto braggQMax = braggReflections.at(nReflections - 1).q();
        Messenger::print("Found reflections data for module '{}' (nReflections = {}, Q(last) = {} "
                         "Angstroms**-1).\n",
                         braggModule->uniqueName(), nReflections, braggQMax);
        const auto &braggAtomTypes =
            dissolve.processingModuleData().value<AtomTypeList>("SummedAtomTypes", braggModule->uniqueName());
        const auto &v0 = dissolve.processingModuleData().value<double>("V0", braggModule->uniqueName());

        // Prepare a temporary object for the Bragg partials
        Array2D<Data1D> braggPartials;
        braggPartials.initialise(unweightedsq.nAtomTypes(), unweightedsq.nAtomTypes(), true);
        for (auto &partial : braggPartials)
            partial.initialise(unweightedsq.partial(0, 0));

        // For each partial in our S(Q) array, calculate the broadened Bragg function and blend it
        auto result = for_each_pair_early(
            unweightedsq.atomTypes().begin(), unweightedsq.atomTypes().end(),
            [&braggReflections, &braggAtomTypes, &braggQBroadening, &braggPartials, &qDelta](auto i, auto &at1, auto j,
                                                                                             auto &at2) -> EarlyReturn<bool> {
                // Locate the corresponding Bragg intensities for this atom type pair
                auto pairIndex = braggAtomTypes.indexOf(at1.atomType(), at2.atomType());
                if (pairIndex.first == -1 || pairIndex.second == -1)
                    return Messenger::error(
                        "SQ data has a partial between {} and {}, but no such intensities exist in the reflection data.\n",
                        at1.atomTypeName(), at2.atomTypeName());

                // Grab relevant partial and oop over reflections
                auto &partial = braggPartials[pairIndex];
                for (const auto &reflxn : braggReflections)
                {
                    const auto intensity = reflxn.intensity(pairIndex.first, pairIndex.second);
                    for (auto &&[q, by] : zip(partial.xAxis(), partial.values()))
                        by += braggQBroadening.y(q - reflxn.q(), q) * intensity * braggQBroadening.normalisation(q) /
                              (reflxn.q() * q);
                }

                return EarlyReturn<bool>::Continue;
            });
        if (result && !result.value())
            return false;

        // Finalise partials
        for (auto &partial : braggPartials)
            std::transform(partial.values().begin(), partial.values().end(), partial.values().begin(),
                           [v0](auto &val) { return val * 2.0 * pow(M_PI, 2) / v0; });

        // Remove self-scattering level from partials between the same atom type and remove normalisation from atomic fractions
        for_each_pair(unweightedsq.atomTypes().begin(), unweightedsq.atomTypes().end(),
                      [&braggPartials](auto i, auto &atd1, auto j, auto &atd2) {
                          // Subtract self-scattering level if types are equivalent
                          if (i == j)
                              braggPartials[{i, j}] -= atd1.fraction();

                          // Remove atomic fraction normalisation
                          braggPartials[{i, j}] /= atd1.fraction() * atd2.fraction();
                      });

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

        // Re-set the object names and fingerprints of the partials
        unweightedsq.setFingerprint(currentFingerprint);
    }

    // Set fingerprint
    unweightedsq.setFingerprint(
        fmt::format("{}/{}", dissolve.processingModuleData().version("UnweightedGR", rdfModule->uniqueName()),
                    braggModule ? dissolve.processingModuleData().version("Reflections", braggModule->uniqueName()) : -1));
    // Save data if requested
    if (saveData && !MPIRunMaster(procPool, unweightedsq.save(uniqueName_, "UnweightedSQ", "sq", "Q, 1/Angstroms")))
        return false;

    return true;
}
