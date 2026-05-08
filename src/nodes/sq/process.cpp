// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/averaging.h"
#include "nodes/sq/sq.h"

// Run main processing
NodeConstants::ProcessResult SQNode::process()
{
    /*
     * Calculate S(Q) from Configuration's g(r).
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     */

    auto qMin = qMin_.asDouble(), qDelta = qDelta_.asDouble(), qMax = qMax_.asDouble();

    // Print argument/parameter summary
    message("SQ: Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin, qMax,
            qDelta);
    if (windowFunction_ == WindowFunction::Form::None)
        message("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        message("SQ: Window function to be applied in Fourier transforms is {}.",
                WindowFunction::forms().keyword(windowFunction_));
    if (averagingLength_)
        message("SQ: Partials will be averaged over {} sets (scheme = {}).\n", averagingLength_.value().asDouble(),
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

    // Set up unweighted SQ storage if we need to
    if (!unweightedSQ_)
    {
        unweightedSQ_.emplace();
        unweightedSQ_.value().initialise(*unweightedGR_);
    }

    /*
     * Transform target UnweightedGR into the UnweightedSQ.
     */

    // Transform g(r) into S(Q)
    if (!calculateUnweightedSQ())
        return NodeConstants::ProcessResult::Failed;

    // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
    if (averagingLength_)
        (*unweightedSQ_) = unweightedSQHistory_.push(*unweightedSQ_, averagingLength_.value().asInteger());

    // Save data if requested
    if (save_ && !unweightedSQ_->save(name(), "UnweightedSQ", "sq", "Q, 1/Angstroms"))
        return NodeConstants::ProcessResult::Failed;

    return NodeConstants::ProcessResult::Success;
}
