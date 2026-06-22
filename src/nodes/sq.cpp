// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/sq.h"
#include "base/timer.h"
#include "math/ft.h"
#include "math/mathFunc.h"
#include "nodes/edge.h"
#include "nodes/gr.h"
#include "templates/algorithms.h"

SQNode::SQNode(Graph *parentGraph)
    : Node(parentGraph), unweightedSQHistory_(
                             [&]()
                             {
                                 PartialSet p;
                                 p.initialise(*unweightedGR_);
                                 return p;
                             })
{
    // Inputs
    addInput("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    // Outputs
    addOptionalPointerOutput<PartialSet>("UnweightedSQ", "Unweighted partials for target configuration", unweightedSQ_);
    addOutput("UnweightedGR", "Unweighted partials for target configuration", unweightedGR_);

    // Options
    addOption("QMin", "Minimum Q for calculated S(Q)", qMin_);
    addOption("QMax", "Maximum Q for calculated S(Q)", qMax_);
    addOption("QDelta", "Step size in Q for S(Q) calculation", qDelta_);
    addOption("QBroadening", "Instrument broadening function to apply when calculating S(Q)", qBroadening_);
    addOption("WindowFunction", "Window function to apply when Fourier-transforming reference S(Q) to g(r)", windowFunction_);
    addOption("Averaging", "Number of historical partial sets to combine into final partials", averagingLength_);

    // Serialisables
    addSerialisable("unweightedSQ", unweightedSQ_);
    addSerialisable("unweightedSQ", unweightedSQHistory_);
}

/*
 * Definition
 */

// Return type of the node
std::string_view SQNode::type() const { return "SQ"; }

// Return short summary of the node's purpose
std::string_view SQNode::summary() const { return "Calculate unweighted structure factors from input G(r)"; }

/*
 * Data
 */

// Returns the unweighted SQ
const PartialSet &SQNode::unweightedSQ() const { return *unweightedSQ_; }

// Returns the unweighted GR
const PartialSet &SQNode::unweightedGR() const { return *unweightedGR_; }

// Returns the source configuration, belonging to the input GR node
const Configuration *SQNode::sourceConfiguration()
{
    auto cfgInputEdge = inputEdges().find("UnweightedGR");

    if (cfgInputEdge == inputEdges().end())
    {
        error("Could not find a valid input 'UnweightedGR' associated with this node ({})", name());
        return nullptr;
    }

    auto &cfgSourceNode = cfgInputEdge->second[0]->sourceNode();
    auto grNode = static_cast<GRNode *>(&cfgSourceNode);

    return grNode->getInputValue<Configuration *>("Configuration");
}

/*
 * Processing
 */

// Perform processing
NodeConstants::ProcessResult SQNode::process()
{
    auto qMin = qMin_.asDouble(), qDelta = qDelta_.asDouble(), qMax = qMax_.asDouble();
    auto rho = unweightedGR_->effectiveDensity();

    // Print argument/parameter summary
    message("Calculating S(Q)/F(Q) over {} < Q < {} Angstroms**-1 using step size of {} Angstroms**-1.\n", qMin, qMax, qDelta);
    if (windowFunction_ == WindowFunction::Form::None)
        message("No window function will be applied in Fourier transforms of g(r) to S(Q).");
    else
        message("Window function to be applied in Fourier transforms is {}.", WindowFunction::forms().keyword(windowFunction_));
    if (averagingLength_)
        message("Partials will be averaged over {} sets.\n", averagingLength_.value().asDouble());
    else
        message("No averaging of partials will be performed.\n");
    if (qBroadening_.form() == Functions1D::Form::None)
        message("No broadening will be applied to calculated S(Q).");
    else
        message("Broadening to be applied in calculated S(Q) is {} ({}).", Functions1D::forms().keyword(qBroadening_.form()),
                qBroadening_.parameterSummary());

    // Set up unweighted SQ storage if we need to
    if (!unweightedSQ_)
    {
        unweightedSQ_.emplace();
        unweightedSQ_.value().initialise(*unweightedGR_);
    }

    // Transform g(r) into S(Q)
    // Subtract 1.0 from the full and unbound partials so as to give (g(r)-1) and FT into S(Q)
    // Don't subtract 1.0 from the bound partials
    Timer timer;

    // Full partials
    dissolve::for_each(ParallelPolicies::par, unweightedGR_->partials().begin(), unweightedGR_->partials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedSQ_->partials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           sq -= 1.0;
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction_, qBroadening_);
                       });

    // Bound partials
    dissolve::for_each(ParallelPolicies::par, unweightedGR_->boundPartials().begin(), unweightedGR_->boundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedSQ_->boundPartials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction_, qBroadening_);
                       });

    // Unbound partials
    dissolve::for_each(ParallelPolicies::par, unweightedGR_->unboundPartials().begin(), unweightedGR_->unboundPartials().end(),
                       [&](const auto &pair)
                       {
                           auto &sq = unweightedSQ_->unboundPartials().map()[pair.first];
                           sq.copyArrays(pair.second);
                           sq -= 1.0;
                           Fourier::sineFT(sq, 4.0 * M_PI * rho, qMin, qDelta, qMax, windowFunction_, qBroadening_);
                       });

    // Sum into total
    unweightedSQ_->formTotals(true);

    timer.stop();
    message("Finished Fourier transform and summation of partial g(r) into partial S(Q) ({} elapsed).\n",
            timer.totalTimeString());

    // Perform averaging of unweighted partials if requested, and if we're not already up-to-date
    if (averagingLength_)
        (*unweightedSQ_) = unweightedSQHistory_.push(*unweightedSQ_, averagingLength_.value().asInteger());

    return NodeConstants::ProcessResult::Success;
}
