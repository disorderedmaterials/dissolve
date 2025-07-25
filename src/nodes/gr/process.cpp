// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
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
    message("Save raw simulation g(r) is {}.\n", DissolveSys::onOff(saveRaw_));
    if (nSmooths_)
        message("Degree of smoothing to apply to calculated partial g(r) is {}.\n", nSmooths_.value().asInteger());
    message("\n");

    // Check range
    auto grRange = targetConfiguration_->box()->inscribedSphereRadius();
    if (!requestedRange_)
        message("Maximal cutoff used for Configuration '{}' ({} Angstroms).\n", targetConfiguration_->niceName(), grRange);
    else
    {
        if (requestedRange_.value_or(Number(0.0)) > grRange)
        {
            error("Specified RDF range of {} Angstroms is out of range for Configuration "
                  "'{}' (max = {} Angstroms).\n",
                  requestedRange_.value().asDouble(), targetConfiguration_->niceName(), grRange);
            return NodeConstants::ProcessResult::Failed;
        }

        grRange = requestedRange_.value().asDouble();
        message("Cutoff for Configuration '{}' is {} Angstroms.\n", targetConfiguration_->niceName(), grRange);
    }

    // 'Snap' grRange to nearest bin width...
    grRange = int(grRange / binWidth_.asDouble()) * binWidth_.asDouble();
    message("Cutoff (snapped to bin width) is {} Angstroms.\n", grRange);

    // Convert configuration species populations into real species populations
    std::map<const Species *, double> realSpeciesPopulations;
    for (auto &[sp, iPop] : targetConfiguration_->speciesPopulations())
        realSpeciesPopulations[sp] = iPop;

    // Create original GR storage if we need it
    if (!rawGR_)
    {
        rawGR_.emplace(realSpeciesPopulations);
        rawGR_->setUp(targetConfiguration_->atomTypePopulations(), grRange, binWidth_.asDouble());
        unweightedGR_->setEffectiveDensity(targetConfiguration_->atomicDensity().value_or(0.0));
    }

    // Calculate unweighted partials for this Configuration
    bool alreadyUpToDate;
    calculateRawGR(grRange, alreadyUpToDate);

    // Perform averagingLength_ of unweighted partials if requested, and if we're not already up-to-date
    /*
    if ((averagingLength_.value_or(1) > 1) && (!alreadyUpToDate))
    {
        // Store the current fingerprint, since we must ensure we retain it in the averaged T.
        std::string currentFingerprint{rawGR_.fingerprint()};

        Averaging::average<PartialSet>(dissolve().processingModuleData(), std::format("{}//OriginalGR",
    targetConfiguration_->niceName()), name(), averagingLength_.value().asDouble(), averagingScheme_);
    }
    */

    /*
    // Perform internal test of original g(r)?
    if (internalTest_)
    {
        // Copy the already-calculated g(r), then calculate a new set using the Test method
        PartialSet referencePartials = originalgr;
        calculateGR(dissolve.processingModuleData(), moduleContext.processPool(), cfg, GRModule::TestMethod,
            grRange, binWidth_, alreadyUpToDate);
        if (!testReferencePartials(referencePartials, originalgr, 1.0e-6))
            return ExecutionResult::Failed;
    }
    */

    // Create unweighted GR storage if we need it
    if (!unweightedGR_)
        unweightedGR_.emplace();

    // Form unweighted g(r) from original g(r), applying any requested smoothing and/or intramolecular broadening
    calculateUnweightedGR();

    return NodeConstants::ProcessResult::Success;
}
