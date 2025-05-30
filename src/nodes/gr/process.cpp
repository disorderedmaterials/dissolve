// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "module/context.h"
#include "modules/gr/gr.h"
#include "nodes/gr/gr.h"

// Run main processing
NodeConstants::ProcessResult GRNode::process()
{
    /*
     * Calculate standard partial g(r)
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    // Print argument/parameter summary
    if (!requestedRange_)
        message("Partials will be calculated up to the half-cell range limit.\n");
    else
        message("Partials will be calculated out to {} Angstroms.\n", requestedRange_.value().asDouble());
    message("Bin-width to use is {} Angstroms.\n", binWidth_.asDouble());
    if (averagingLength_)
        message("Partials will be averaged over {} sets (scheme = {}).\n", averagingLength_.value().asDouble(),
                Averaging::averagingSchemes().keyword(averagingScheme_));
    else
        message("No averaging of partials will be performed.\n");
    if (intraBroadening_.form() == Functions1D::Form::None)
        message("No broadening will be applied to intramolecular g(r).");
    else
        message("Broadening to be applied to intramolecular g(r) is {} ({}).",
                Functions1D::forms().keyword(intraBroadening_.form()), intraBroadening_.parameterSummary());
    message("Calculation method is '{}'.\n", partialsMethods().keyword(partialsMethod_));
    message("Save data is {}.\n", DissolveSys::onOff(save_));
    message("Save original (unbroadened) g(r) is {}.\n", DissolveSys::onOff(saveOriginal_));
    if (nSmooths_)
        message("Degree of smoothing to apply to calculated partial g(r) is {}.\n", nSmooths_.value().asInteger());
    message("\n");

    /*
     * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or
     * multiple independent Configurations, we must loop over the specified targetConfigurations_ and calculate the partials
     * for each.
     */

    for (auto *cfg : targetConfigurations_)
    {
        // Check RDF range
        double rdfRange = cfg->box()->inscribedSphereRadius();
        if (!requestedRange_)
            message("Maximal cutoff used for Configuration '{}' ({} Angstroms).\n", cfg->name(), rdfRange);
        else
        {
            if (requestedRange_.value_or(Number(0.0)) > rdfRange)
            {
                error("Specified RDF range of {} Angstroms is out of range for Configuration "
                      "'{}' (max = {} Angstroms).\n",
                      requestedRange_.value().asDouble(), cfg->name(), rdfRange);
                return NodeConstants::ProcessResult::Failed;
            }

            rdfRange = requestedRange_.value().asDouble();
            message("Cutoff for Configuration '{}' is {} Angstroms.\n", cfg->name(), rdfRange);
        }

        // 'Snap' rdfRange_ to nearest bin width...
        rdfRange = int(rdfRange / binWidth_.asDouble()) * binWidth_.asDouble();
        message("Cutoff (snapped to bin width) is {} Angstroms.\n", rdfRange);

        // Calculate unweighted partials for this Configuration
        bool alreadyUpToDate;
        calculateGR(dissolve().processingModuleData(), processPool(), cfg, partialsMethod_, rdfRange, binWidth_.asDouble(),
                    alreadyUpToDate);
        auto &originalgr =
            dissolve().processingModuleData().retrieve<PartialSet>(std::format("{}//OriginalGR", cfg->name()), name());

        // Perform averagingLength_ of unweighted partials if requested, and if we're not already up-to-date
        if ((averagingLength_.value_or(1) > 1) && (!alreadyUpToDate))
        {
            // Store the current fingerprint, since we must ensure we retain it in the averaged T.
            std::string currentFingerprint{originalgr.fingerprint()};

            Averaging::average<PartialSet>(dissolve().processingModuleData(), std::format("{}//OriginalGR", cfg->name()),
                                           name(), averagingLength_.value().asDouble(), averagingScheme_);

            // Re-set the object names and fingerprints of the partials
            originalgr.setFingerprint(currentFingerprint);
        }

        // Perform internal test of original g(r)?
        if (internalTest_)
        {
            // Copy the already-calculated g(r), then calculate a new set using the Test method
            PartialSet referencePartials = originalgr;
            calculateGR(dissolve().processingModuleData(), processPool(), cfg, PartialsMethod::TestMethod, rdfRange,
                        binWidth_.asDouble(), alreadyUpToDate);
            if (!testReferencePartials(referencePartials, originalgr, 1.0e-6))
                return NodeConstants::ProcessResult::Failed;
        }

        // Form unweighted g(r) from original g(r), applying any requested nSmooths_.asInteger() / intramolecular broadening
        auto &unweightedgr = dissolve().processingModuleData().realise<PartialSet>(
            std::format("{}//UnweightedGR", cfg->name()), name(), GenericItem::InRestartFileFlag);
        calculateUnweightedGR(processPool(), cfg, originalgr, unweightedgr, intraBroadening_,
                              nSmooths_.value_or(0).asInteger());

        // Save data if requested
        if (save_ && (!MPIRunMaster(processPool(), unweightedgr.save(name(), "UnweightedGR", "gr", "r, Angstroms"))))
            return NodeConstants::ProcessResult::Failed;
        if (saveOriginal_ && (!MPIRunMaster(processPool(), originalgr.save(name(), "OriginalGR", "gr", "r, Angstroms"))))
            return NodeConstants::ProcessResult::Failed;
    }

    // Create/retrieve PartialSet for summed unweighted g(r)
    auto &summedUnweightedGR =
        dissolve().processingModuleData().realise<PartialSet>("UnweightedGR", name(), GenericItem::InRestartFileFlag);

    // Sum the partials from the associated Configurations
    if (!sumUnweightedGR(dissolve().processingModuleData(), processPool(), name(), name(), targetConfigurations_,
                         summedUnweightedGR))
        return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
