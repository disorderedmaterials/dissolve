// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "modules/gr/gr.h"

// Run main processing
Module::ExecutionResult GRModule::process(Dissolve &dissolve)
{
    // Print argument/parameter summary
    if (!requestedRange_)
        Messenger::print("RDF: Partials will be calculated up to the half-cell range limit.\n");
    else
        Messenger::print("RDF: Partials will be calculated out to {} Angstroms.\n", requestedRange_.value());
    Messenger::print("RDF: Bin-width to use is {} Angstroms.\n", binWidth_);
    if (averagingLength_)
        Messenger::print("RDF: Partials will be averaged over {} sets (scheme = {}).\n", averagingLength_.value(),
                         Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        Messenger::print("RDF: No averaging of partials will be performed.\n");
    if (intraBroadening_.form() == Functions1D::Form::None)
        Messenger::print("RDF: No broadening will be applied to intramolecular g(r).");
    else
        Messenger::print("RDF: Broadening to be applied to intramolecular g(r) is {} ({}).",
                         Functions1D::forms().keyword(intraBroadening_.form()), intraBroadening_.parameterSummary());
    Messenger::print("RDF: Calculation method is '{}'.\n", partialsMethods().keyword(partialsMethod_));
    Messenger::print("RDF: Save data is {}.\n", DissolveSys::onOff(save_));
    Messenger::print("RDF: Save original (unbroadened) g(r) is {}.\n", DissolveSys::onOff(saveOriginal_));
    if (nSmooths_)
        Messenger::print("RDF: Degree of smoothing to apply to calculated partial g(r) is {}.\n", *nSmooths_);
    Messenger::print("\n");

    /*
     * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or
     * multiple independent Configurations, we must loop over the specified targetConfiguration_ and calculate the partials
     * for each.
     */

    for (auto *cfg : targetConfigurations_)
    {
        // Check RDF range
        double rdfRange = cfg->box()->inscribedSphereRadius();
        if (!requestedRange_)
            Messenger::print("Maximal cutoff used for Configuration '{}' ({} Angstroms).\n", cfg->name(), rdfRange);
        else
        {
            if (requestedRange_.value_or(0.0) > rdfRange)
            {
                Messenger::error("Specified RDF range of {} Angstroms is out of range for Configuration "
                                 "'{}' (max = {} Angstroms).\n",
                                 requestedRange_.value(), cfg->name(), rdfRange);
                return ExecutionResult::Failed;
            }

            rdfRange = requestedRange_.value();
            Messenger::print("Cutoff for Configuration '{}' is {} Angstroms.\n", cfg->name(), rdfRange);
        }

        // 'Snap' rdfRange_ to nearest bin width...
        rdfRange = int(rdfRange / binWidth_) * binWidth_;
        Messenger::print("Cutoff (snapped to bin width) is {} Angstroms.\n", rdfRange);

        // Calculate unweighted partials for this Configuration
        bool alreadyUpToDate;
        calculateGR(dissolve.processingModuleData(), cfg, partialsMethod_, rdfRange, binWidth_, alreadyUpToDate);
        auto &originalgr =
            dissolve.processingModuleData().retrieve<PartialSet>(std::format("{}//OriginalGR", cfg->name()), name_);

        // Perform averagingLength_ of unweighted partials if requested, and if we're not already up-to-date
        if ((averagingLength_.value_or(1) > 1) && (!alreadyUpToDate))
        {
            // Store the current fingerprint, since we must ensure we retain it in the averaged T.
            std::string currentFingerprint{originalgr.fingerprint()};

            Averaging::average<PartialSet>(dissolve.processingModuleData(), std::format("{}//OriginalGR", cfg->name()),
                                           name_, averagingLength_.value(), averagingScheme_);

            // Re-set the object names and fingerprints of the partials
            originalgr.setFingerprint(currentFingerprint);
        }

        // Perform internal test of original g(r)?
        if (internalTest_)
        {
            // Copy the already-calculated g(r), then calculate a new set using the Test method
            PartialSet referencePartials = originalgr;
            calculateGR(dissolve.processingModuleData(), cfg, GRModule::TestMethod, rdfRange, binWidth_, alreadyUpToDate);
            if (!testReferencePartials(cfg->atomTypeVector(), referencePartials, originalgr, 1.0e-6))
                return ExecutionResult::Failed;
        }

        // Form unweighted g(r) from original g(r), applying any requested nSmooths_ / intramolecular broadening
        auto &unweightedgr = dissolve.processingModuleData().realise<PartialSet>(
            std::format("{}//UnweightedGR", cfg->name()), name_, GenericItem::InRestartFileFlag);
        calculateUnweightedGR(cfg, originalgr, unweightedgr, intraBroadening_, nSmooths_.value_or(0));

        // Save data if requested
        if (save_ && (!unweightedgr.save(name_, "UnweightedGR", "gr", "r, Angstroms")))
            return ExecutionResult::Failed;
        if (saveOriginal_ && (!originalgr.save(name_, "OriginalGR", "gr", "r, Angstroms")))
            return ExecutionResult::Failed;
    }

    // Create/retrieve PartialSet for summed unweighted g(r)
    auto &summedUnweightedGR =
        dissolve.processingModuleData().realise<PartialSet>("UnweightedGR", name_, GenericItem::InRestartFileFlag);

    // Sum the partials from the associated Configurations
    if (!GRModule::sumUnweightedGR(dissolve.processingModuleData(), name(), name(), targetConfigurations_, summedUnweightedGR))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
