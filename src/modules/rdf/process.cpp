// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/box.h"
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
    if (targetConfigurations_.empty())
        return Messenger::error("No configuration targets set for module '{}'.\n", uniqueName());

    // Print argument/parameter summary
    if (useHalfCellRange_)
        Messenger::print("RDF: Partials will be calculated up to the half-cell range limit.\n");
    else
        Messenger::print("RDF: Partials will be calculated out to {} Angstroms.\n", requestedRange_);
    Messenger::print("RDF: Bin-width to use is {} Angstroms.\n", binWidth_);
    if (averagingLength_ <= 1)
        Messenger::print("RDF: No averaging of partials will be performed.\n");
    else
        Messenger::print("RDF: Partials will be averaged over {} sets (scheme = {}).\n", averagingLength_,
                         Averaging::averagingSchemes().keyword(averagingScheme_));
    if (intraBroadening_.type() == Functions::Function1D::None)
        Messenger::print("RDF: No broadening will be applied to intramolecular g(r).");
    else
        Messenger::print("RDF: Broadening to be applied to intramolecular g(r) is {} ({}).",
                         Functions::function1D().keyword(intraBroadening_.type()), intraBroadening_.parameterSummary());
    Messenger::print("RDF: Calculation method is '{}'.\n", partialsMethods().keyword(partialsMethod_));
    Messenger::print("RDF: Save data is {}.\n", DissolveSys::onOff(save_));
    Messenger::print("RDF: Degree of smoothing to apply to calculated partial g(r) is {} ({}).\n", nSmooths_,
                     DissolveSys::onOff(nSmooths_ > 0));
    Messenger::print("\n");

    /*
     * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or
     * multiple independent Configurations, we must loop over the specified targetConfigurations_ and calculate the partials
     * for each.
     */

    // Check that Configurations have unmodified size factor
    if (std::all_of(targetConfigurations_.begin(), targetConfigurations_.end(), [](const auto *cfg) {
            return std::abs(cfg->appliedSizeFactor() - 1.0) > 2 * std::numeric_limits<double>::epsilon();
        }))
    {
        Messenger::print("One or more configurations have an applied size factor, so RDF calculation will be skipped.\n");
        return true;
    }

    for (auto *cfg : targetConfigurations_)
    {
        // Set up process pool - must do this to ensure we are using all available processes
        procPool.assignProcessesToGroups(cfg->processPool());

        // Check RDF range
        double rdfRange = cfg->box()->inscribedSphereRadius();
        if (useHalfCellRange_)
        {
            Messenger::print("Maximal cutoff used for Configuration '{}' ({} Angstroms).\n", cfg->niceName(), rdfRange);
        }
        else
        {
            if (requestedRange_ > rdfRange)
                return Messenger::error("Specified RDF range of {} Angstroms is out of range for Configuration "
                                        "'{}' (max = {} Angstroms).\n",
                                        requestedRange_, cfg->niceName(), rdfRange);
            rdfRange = requestedRange_;
            Messenger::print("Cutoff for Configuration '{}' is {} Angstroms.\n", cfg->niceName(), rdfRange);
        }

        // 'Snap' rdfRange_ to nearest bin width...
        rdfRange = int(rdfRange / binWidth_) * binWidth_;
        Messenger::print("Cutoff (snapped to bin width) is {} Angstroms.\n", rdfRange);

        // Calculate unweighted partials for this Configuration
        bool alreadyUpToDate;
        calculateGR(dissolve.processingModuleData(), procPool, cfg, partialsMethod_, rdfRange, binWidth_, alreadyUpToDate);
        auto &originalgr =
            dissolve.processingModuleData().retrieve<PartialSet>(fmt::format("{}//OriginalGR", cfg->niceName()), uniqueName_);

        // Perform averagingLength_ of unweighted partials if requested, and if we're not already up-to-date
        if ((averagingLength_ > 1) && (!alreadyUpToDate))
        {
            // Store the current fingerprint, since we must ensure we retain it in the averaged T.
            std::string currentFingerprint{originalgr.fingerprint()};

            Averaging::average<PartialSet>(dissolve.processingModuleData(), fmt::format("{}//OriginalGR", cfg->niceName()),
                                           uniqueName_, averagingLength_, averagingScheme_);

            // Re-set the object names and fingerprints of the partials
            originalgr.setFingerprint(currentFingerprint);
        }

        // Perform internal test of original g(r)?
        if (internalTest_)
        {
            // Copy the already-calculated g(r), then calculate a new set using the Test method
            PartialSet referencePartials = originalgr;
            calculateGR(dissolve.processingModuleData(), procPool, cfg, RDFModule::TestMethod, rdfRange, binWidth_,
                        alreadyUpToDate);
            if (!testReferencePartials(referencePartials, originalgr, 1.0e-6))
                return false;
        }

        // Form unweighted g(r) from original g(r), applying any requested nSmooths_ / intramolecular broadening
        auto &unweightedgr = dissolve.processingModuleData().realise<PartialSet>(
            fmt::format("{}//UnweightedGR", cfg->niceName()), uniqueName_, GenericItem::InRestartFileFlag);
        calculateUnweightedGR(procPool, cfg, originalgr, unweightedgr, intraBroadening_, nSmooths_);

        // Save data if requested
        if (save_ && (!MPIRunMaster(procPool, unweightedgr.save(uniqueName_, "UnweightedGR", "gr", "r, Angstroms"))))
            return false;
    }

    // Create/retrieve PartialSet for summed unweighted g(r)
    auto &summedUnweightedGR =
        dissolve.processingModuleData().realise<PartialSet>("UnweightedGR", uniqueName_, GenericItem::InRestartFileFlag);

    // Sum the partials from the associated Configurations
    if (!RDFModule::sumUnweightedGR(dissolve.processingModuleData(), procPool, uniqueName(), uniqueName(),
                                    targetConfigurations_, summedUnweightedGR))
        return false;

    return true;
}
