// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/collect1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "keywords/types.h"
#include "math/data1d.h"
#include "procedure/nodes/calculatebase.h"
#include "procedure/nodes/sequence.h"

Collect1DProcedureNode::Collect1DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> observable, double rMin, double rMax,
                                               double binWidth)
    : ProcedureNode(ProcedureNode::NodeType::Collect1D), xObservable_{observable, 0}, rangeX_{rMin, rMax, binWidth}
{
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>("Control", "QuantityX", "Calculated observable to collect",
                                                                     xObservable_, this, ProcedureNode::NodeClass::Calculate,
                                                                     true);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeX", "Range and binwidth of the x-axis of the histogram", rangeX_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.addKeyword<NodeBranchKeyword>("SubCollect", "Branch which runs if the target quantity was binned successfully",
                                            subCollectBranch_, this, ProcedureNode::AnalysisContext);

    // Initialise branch
    subCollectBranch_ = nullptr;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return current data
Data1D Collect1DProcedureNode::data() const
{
    assert(histogram_);

    return histogram_->get().data();
}

// Return accumulated data
const Data1D &Collect1DProcedureNode::accumulatedData() const
{
    assert(histogram_);

    return histogram_->get().accumulatedData();
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
std::shared_ptr<SequenceProcedureNode> Collect1DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = std::make_shared<SequenceProcedureNode>(context, procedure());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect1DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
std::shared_ptr<SequenceProcedureNode> Collect1DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), cfg->niceName());
    auto [target, status] = targetList.realiseIf<Histogram1D>(dataName, prefix, GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        Messenger::printVerbose("One-dimensional histogram data for '{}' was not in the target list, so it will now be "
                                "initialised...\n",
                                name());
        target.initialise(rangeX_.x, rangeX_.y, rangeX_.z);
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a reference to the data
    histogram_ = target;

    // Check target observable
    if (!xObservable_.first)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());

    // Prepare any branches
    if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList)))
        return false;

    return true;
}

// Execute node, targetting the supplied Configuration
bool Collect1DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                     GenericList &targetList)
{
    auto [observable, index] = xObservable_;

    assert(observable && histogram_);

    // Bin the current value of the observable, and execute sub-collection branch on success
    if (histogram_->get().bin(observable->value(index)) && subCollectBranch_)
        return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

    return true;
}

// Finalise any necessary data after execution
bool Collect1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    assert(histogram_);

    // Accumulate the current binned data
    histogram_->get().accumulate();

    // Finalise any branches
    if (subCollectBranch_ && (!subCollectBranch_->finalise(procPool, cfg, prefix, targetList)))
        return false;

    return true;
}
