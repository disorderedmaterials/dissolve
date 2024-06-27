// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "math/averaging.h"
#include "module/context.h"
#include "modules/tr/tr.h"

// Run main processing
Module::ExecutionResult TRModule::process(ModuleContext &moduleContext)
{
    /*
     * Calculate standard partial g(r)
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

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
     * multiple independent Configurations, we must loop over the specified targetConfigurations_ and calculate the partials
     * for each.
     */

    for (auto *cfg : targetConfigurations_)
    {
        return ExecutionResult::Success;
    }
}
