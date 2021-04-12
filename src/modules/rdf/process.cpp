// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/neutronweights.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "modules/rdf/rdf.h"

// Run main processing
bool RDFModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate standard partial g(r)
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    // Check for zero Configuration targets
    if (targetConfigurations_.nItems() == 0)
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    const auto averaging = keywords_.asInt("Averaging");
    auto averagingScheme = keywords_.enumeration<Averaging::AveragingScheme>("AveragingScheme");
    auto &intraBroadening = keywords_.retrieve<PairBroadeningFunction>("IntraBroadening", PairBroadeningFunction());
    auto method = keywords_.enumeration<RDFModule::PartialsMethod>("Method");
    const auto useHalfCellRange = keywords_.asBool("UseHalfCellRange");
    const auto specifiedRange = keywords_.asDouble("Range");
    const auto binWidth = keywords_.asDouble("BinWidth");
    const bool internalTest = keywords_.asBool("InternalTest");
    const bool saveData = keywords_.asBool("Save");
    const auto smoothing = keywords_.asInt("Smoothing");

    // Print argument/parameter summary
    if (useHalfCellRange)
        Messenger::print("RDF: Partials will be calculated up to the half-cell range limit.\n");
    else
        Messenger::print("RDF: Partials will be calculated out to {} Angstroms.\n", specifiedRange);
    Messenger::print("RDF: Bin-width to use is {} Angstroms.\n", binWidth);
    if (averaging <= 1)
        Messenger::print("RDF: No averaging of partials will be performed.\n");
    else
        Messenger::print("RDF: Partials will be averaged over {} sets (scheme = {}).\n", averaging,
                         Averaging::averagingSchemes().keyword(averagingScheme));
    if (intraBroadening.function() == PairBroadeningFunction::NoFunction)
        Messenger::print("RDF: No broadening will be applied to intramolecular g(r).");
    else
        Messenger::print("RDF: Broadening to be applied to intramolecular g(r) is {}.", intraBroadening.summary());
    Messenger::print("RDF: Calculation method is '{}'.\n", partialsMethods().keyword(method));
    Messenger::print("RDF: Save data is {}.\n", DissolveSys::onOff(saveData));
    Messenger::print("RDF: Degree of smoothing to apply to calculated partial g(r) is {} ({}).\n", smoothing,
                     DissolveSys::onOff(smoothing > 0));
    Messenger::print("\n");

    /*
     * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or
     * multiple independent Configurations, we must loop over the specified targetConfigurations_ and calculate the partials
     * for each.
     */
    for (Configuration *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Check RDF range
        double rdfRange = cfg->box()->inscribedSphereRadius();
        if (useHalfCellRange)
        {
            Messenger::print("Maximal cutoff used for Configuration '{}' ({} Angstroms).\n", cfg->niceName(), rdfRange);
        }
        else
        {
            if (specifiedRange > rdfRange)
                return Messenger::error("Specified RDF range of {} Angstroms is out of range for Configuration "
                                        "'{}' (max = {} Angstroms).\n",
                                        specifiedRange, cfg->niceName(), rdfRange);
            rdfRange = specifiedRange;
            Messenger::print("Cutoff for Configuration '{}' is {} Angstroms.\n", cfg->niceName(), rdfRange);
        }

        // 'Snap' rdfRange_ to nearest bin width...
        rdfRange = int(rdfRange / binWidth) * binWidth;
        Messenger::print("Cutoff (snapped to bin width) is {} Angstroms.\n", rdfRange);

        // Calculate unweighted partials for this Configuration
        bool alreadyUpToDate;
        calculateGR(dissolve.processingModuleData(), procPool, cfg, method, rdfRange, binWidth, alreadyUpToDate);
        auto &originalgr =
            dissolve.processingModuleData().retrieve<PartialSet>(fmt::format("{}//OriginalGR", cfg->niceName()), uniqueName_);

        // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
        if ((averaging > 1) && (!alreadyUpToDate))
        {
            // Store the current fingerprint, since we must ensure we retain it in the averaged T.
            std::string currentFingerprint{originalgr.fingerprint()};

            Averaging::average<PartialSet>(dissolve.processingModuleData(), fmt::format("{}//OriginalGR", cfg->niceName()),
                                           uniqueName_, averaging, averagingScheme);

            // Re-set the object names and fingerprints of the partials
            originalgr.setFingerprint(currentFingerprint);
        }

        // Perform internal test of original g(r)?
        if (internalTest)
        {
            // Copy the already-calculated g(r), then calculate a new set using the Test method
            PartialSet referencePartials = originalgr;
            calculateGR(dissolve.processingModuleData(), procPool, cfg, RDFModule::TestMethod, rdfRange, binWidth,
                        alreadyUpToDate);
            if (!testReferencePartials(referencePartials, originalgr, 1.0e-6))
                return false;
        }

        // Form unweighted g(r) from original g(r), applying any requested smoothing / intramolecular broadening
        auto &unweightedgr = dissolve.processingModuleData().realise<PartialSet>(
            fmt::format("{}//UnweightedGR", cfg->niceName()), uniqueName_, GenericItem::InRestartFileFlag);
        calculateUnweightedGR(procPool, cfg, originalgr, unweightedgr, intraBroadening, smoothing);

        // Save data if requested
        if (saveData && (!MPIRunMaster(procPool, unweightedgr.save(uniqueName_, "UnweightedGR", "gr", "r, Angstroms"))))
            return false;
    }

    // Create/retrieve PartialSet for summed unweighted g(r)
    auto &summedUnweightedGR =
        dissolve.processingModuleData().realise<PartialSet>("UnweightedGR", uniqueName_, GenericItem::InRestartFileFlag);

    // Sum the partials from the associated Configurations
    if (!RDFModule::sumUnweightedGR(dissolve.processingModuleData(), procPool, this, this, summedUnweightedGR))
        return false;

    return true;
}
