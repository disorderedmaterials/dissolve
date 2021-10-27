// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/collect2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "keywords/types.h"
#include "math/data2d.h"
#include "procedure/nodes/calculatebase.h"
#include "procedure/nodes/sequence.h"

Collect2DProcedureNode::Collect2DProcedureNode(CalculateProcedureNodeBase *xObservable, CalculateProcedureNodeBase *yObservable,
                                               double xMin, double xMax, double xBinWidth, double yMin, double yMax,
                                               double yBinWidth)
    : ProcedureNode(ProcedureNode::NodeType::Collect2D), rangeX_{xMin, xMax, xBinWidth}, rangeY_{yMin, yMax, yBinWidth}
{
    keywords_.add("Control", new NodeAndIntegerKeyword(this, ProcedureNode::NodeClass::Calculate, true, xObservable, 0),
                  "QuantityX", "Calculated observable to collect for x axis");
    keywords_.add("Control", new NodeAndIntegerKeyword(this, ProcedureNode::NodeClass::Calculate, true, yObservable, 0),
                  "QuantityY", "Calculated observable to collect for y axis");
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeX", "Range and binwidth of the x-axis of the histogram", rangeX_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeY", "Range and binwidth of the y-axis of the histogram", rangeY_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &subCollectBranch_, ProcedureNode::AnalysisContext), "SubCollect",
                  "Branch which runs if the target quantities were binned successfully");

    // Initialise branch
    subCollectBranch_ = nullptr;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect2DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return accumulated data
const Data2D &Collect2DProcedureNode::accumulatedData() const
{
    assert(histogram_);

    return histogram_->get().accumulatedData();
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode *Collect2DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = new SequenceProcedureNode(context, procedure());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect2DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *Collect2DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect2DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), cfg->niceName());
    auto [target, status] = targetList.realiseIf<Histogram2D>(dataName, prefix, GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        Messenger::printVerbose("Two-dimensional histogram data for '{}' was not in the target list, so it will now be "
                                "initialised...\n",
                                name());
        target.initialise(rangeX_.x, rangeX_.y, rangeX_.z, rangeY_.x, rangeY_.y, rangeY_.z);
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a reference to the data
    histogram_ = target;

    // Retrieve the observables
    const ProcedureNode *node;
    std::tie(node, xObservableIndex_) = keywords_.retrieve<std::pair<const ProcedureNode *, int>>("QuantityX");
    xObservable_ = dynamic_cast<const CalculateProcedureNodeBase *>(node);
    if (!xObservable_)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());
    std::tie(node, yObservableIndex_) = keywords_.retrieve<std::pair<const ProcedureNode *, int>>("QuantityY");
    yObservable_ = dynamic_cast<const CalculateProcedureNodeBase *>(node);
    if (!yObservable_)
        return Messenger::error("No valid y quantity set in '{}'.\n", name());

    // Prepare any branches
    if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList)))
        return false;

    return true;
}

// Execute node, targetting the supplied Configuration
bool Collect2DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                     GenericList &targetList)
{
    assert(xObservable_ && yObservable_ && histogram_);

    // Bin the current value of the observable
    if (histogram_->get().bin(xObservable_->value(xObservableIndex_), yObservable_->value(yObservableIndex_)) &&
        subCollectBranch_)
        return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

    return true;
}

// Finalise any necessary data after execution
bool Collect2DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
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
