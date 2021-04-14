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

Collect1DProcedureNode::Collect1DProcedureNode(CalculateProcedureNodeBase *observable, double rMin, double rMax,
                                               double binWidth)
    : ProcedureNode(ProcedureNode::NodeType::Collect1D)
{
    keywords_.add("Control",
                  new NodeAndIntegerKeyword<CalculateProcedureNodeBase>(this, ProcedureNode::NodeType::CalculateBase, true,
                                                                        observable, 0),
                  "QuantityX", "Calculated observable to collect");
    keywords_.add("Control",
                  new Vec3DoubleKeyword(Vec3<double>(rMin, rMax, binWidth), Vec3<double>(0.0, 0.0, 1.0e-5),
                                        Vec3Labels::MinMaxBinwidthlabels),
                  "RangeX", "Range and binwidth of the histogram for QuantityX");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &subCollectBranch_, ProcedureNode::AnalysisContext), "SubCollect",
                  "Branch which runs if the target quantity was binned successfully");

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

// Return accumulated data
const Data1D &Collect1DProcedureNode::accumulatedData() const
{
    assert(histogram_);

    return histogram_->get().accumulatedData();
}

// Return range minimum
double Collect1DProcedureNode::minimum() const { return keywords_.asVec3Double("RangeX").x; }

// Return range maximum
double Collect1DProcedureNode::maximum() const { return keywords_.asVec3Double("RangeX").y; }

// Return bin width
double Collect1DProcedureNode::binWidth() const { return keywords_.asVec3Double("RangeX").z; }

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode *Collect1DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = new SequenceProcedureNode(context, procedure());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect1DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *Collect1DProcedureNode::branch() { return subCollectBranch_; }

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
        target.initialise(minimum(), maximum(), binWidth());
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a reference to the data
    histogram_ = target;

    // Retrieve the observable
    std::tie(xObservable_, xObservableIndex_) = keywords_.retrieve<std::tuple<CalculateProcedureNodeBase *, int>>("QuantityX");
    if (!xObservable_)
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
    assert(xObservable_ && histogram_);

    // Bin the current value of the observable, and execute sub-collection branch on success
    if (histogram_->get().bin(xObservable_->value(xObservableIndex_)) && subCollectBranch_)
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
