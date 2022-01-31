// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/collect3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/configuration.h"
#include "keywords/nodeandinteger.h"
#include "keywords/nodebranch.h"
#include "keywords/vec3double.h"
#include "math/data3d.h"
#include "procedure/nodes/calculatebase.h"
#include "procedure/nodes/sequence.h"

Collect3DProcedureNode::Collect3DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> xObservable,
                                               std::shared_ptr<CalculateProcedureNodeBase> yObservable,
                                               std::shared_ptr<CalculateProcedureNodeBase> zObservable, double xMin,
                                               double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth,
                                               double zMin, double zMax, double zBinWidth)
    : ProcedureNode(ProcedureNode::NodeType::Collect3D), xObservable_{xObservable, 0}, yObservable_{yObservable, 0},
      zObservable_{zObservable, 0}, rangeX_{xMin, xMax, xBinWidth}, rangeY_{yMin, yMax, yBinWidth}, rangeZ_{zMin, zMax,
                                                                                                            zBinWidth}
{
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "Control", "QuantityX", "Calculated observable to collect for x axis", xObservable_, this,
        ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "Control", "QuantityY", "Calculated observable to collect for y axis", yObservable_, this,
        ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "Control", "QuantityZ", "Calculated observable to collect for z axis", zObservable_, this,
        ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeX", "Range and binwidth of the x-axis of the histogram", rangeX_,
                                     Vec3<double>(-1.0e6, -1.0e6, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeY", "Range and binwidth of the y-axis of the histogram", rangeY_,
                                     Vec3<double>(-1.0e6, -1.0e6, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeZ", "Range and binwidth of the z-axis of the histogram", rangeZ_,
                                     Vec3<double>(-1.0e6, -1.0e6, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.addKeyword<NodeBranchKeyword>("SubCollect", "Branch which runs if the target quantities were binned successfully",
                                            subCollectBranch_, this, ProcedureNode::AnalysisContext);

    // Initialise branch
    subCollectBranch_ = nullptr;
}
Collect3DProcedureNode::Collect3DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> xyzObservable, double xMin,
                                               double xMax, double xBinWidth, double yMin, double yMax, double yBinWidth,
                                               double zMin, double zMax, double zBinWidth)
    : ProcedureNode(ProcedureNode::NodeType::Collect3D), xObservable_{xyzObservable, 0}, yObservable_{xyzObservable, 1},
      zObservable_{xyzObservable, 2}, rangeX_{xMin, xMax, xBinWidth}, rangeY_{yMin, yMax, yBinWidth}, rangeZ_{zMin, zMax,
                                                                                                              zBinWidth}
{
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "Control", "QuantityX", "Calculated observable to collect for x axis", xObservable_, this,
        ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "Control", "QuantityY", "Calculated observable to collect for y axis", yObservable_, this,
        ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "Control", "QuantityZ", "Calculated observable to collect for z axis", zObservable_, this,
        ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeX", "Range and binwidth of the x-axis of the histogram", rangeX_,
                                     Vec3<double>(-1.0e6, -1.0e6, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeY", "Range and binwidth of the y-axis of the histogram", rangeY_,
                                     Vec3<double>(-1.0e6, -1.0e6, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("Control", "RangeZ", "Range and binwidth of the z-axis of the histogram", rangeZ_,
                                     Vec3<double>(-1.0e6, -1.0e6, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.addKeyword<NodeBranchKeyword>("SubCollect", "Branch which runs if the target quantities were binned successfully",
                                            subCollectBranch_, this, ProcedureNode::AnalysisContext);

    // Initialise branch
    subCollectBranch_ = nullptr;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Collect3DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return accumulated data
const Data3D &Collect3DProcedureNode::accumulatedData() const
{
    assert(histogram_);

    return histogram_->get().accumulatedData();
}

/*
 * Branches
 */

// Add and return subcollection sequence branch
std::shared_ptr<SequenceProcedureNode> Collect3DProcedureNode::addSubCollectBranch(ProcedureNode::NodeContext context)
{
    if (!subCollectBranch_)
        subCollectBranch_ = std::make_shared<SequenceProcedureNode>(context, procedure());

    subCollectBranch_->setParent(shared_from_this());

    return subCollectBranch_;
}

// Return whether this node has a branch
bool Collect3DProcedureNode::hasBranch() const { return (subCollectBranch_ != nullptr); }
// Return SequenceNode for the branch (if it exists)
std::shared_ptr<SequenceProcedureNode> Collect3DProcedureNode::branch() { return subCollectBranch_; }

std::vector<ConstNodeRef> Collect3DProcedureNode::children() const { return {subCollectBranch_}; }
/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect3DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), cfg->niceName());
    auto [target, status] = targetList.realiseIf<Histogram3D>(dataName, prefix, GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        Messenger::printVerbose("Three-dimensional histogram data for '{}' was not in the target list, so it will now "
                                "be initialised...\n",
                                name());
        target.initialise(rangeX_.x, rangeX_.y, rangeX_.z, rangeY_.x, rangeY_.y, rangeY_.z, rangeZ_.x, rangeZ_.y, rangeZ_.z);
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a reference to the data
    histogram_ = target;

    // Check target observables
    if (!xObservable_.first)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());
    if (!yObservable_.first)
        return Messenger::error("No valid y quantity set in '{}'.\n", name());
    if (!zObservable_.first)
        return Messenger::error("No valid z quantity set in '{}'.\n", name());

    // Prepare any branches
    if (subCollectBranch_ && (!subCollectBranch_->prepare(cfg, prefix, targetList)))
        return false;

    return true;
}

// Execute node, targetting the supplied Configuration
bool Collect3DProcedureNode::execute(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                     GenericList &targetList)
{
    auto [xObs, xIndex] = xObservable_;
    auto [yObs, yIndex] = yObservable_;
    auto [zObs, zIndex] = zObservable_;

    assert(xObs && yObs && zObs && histogram_);

    // Bin the current value of the observable
    if (histogram_->get().bin(xObs->value(xIndex), yObs->value(yIndex), zObs->value(zIndex)) && subCollectBranch_)
        return subCollectBranch_->execute(procPool, cfg, prefix, targetList);

    return true;
}

// Finalise any necessary data after execution
bool Collect3DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
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
