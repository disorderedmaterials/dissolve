// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/collect2D.h"
#include "classes/configuration.h"
#include "keywords/nodeAndInteger.h"
#include "keywords/nodeBranch.h"
#include "keywords/vec3Double.h"
#include "math/data2D.h"
#include "procedure/nodes/calculateBase.h"
#include "procedure/nodes/sequence.h"

Collect2DProcedureNode::Collect2DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> xObservable,
                                               std::shared_ptr<CalculateProcedureNodeBase> yObservable,
                                               ProcedureNode::NodeContext subCollectContext, double xMin, double xMax,
                                               double xBinWidth, double yMin, double yMax, double yBinWidth)
    : ProcedureNode(ProcedureNode::NodeType::Collect2D, {ProcedureNode::AnalysisContext}), xObservable_{xObservable, 0},
      yObservable_{yObservable, 0}, rangeX_{xMin, xMax, xBinWidth}, rangeY_{yMin, yMax, yBinWidth},
      subCollectBranch_(subCollectContext, *this, "SubCollect")
{
    keywords_.setOrganisation("Options", "Quantities / Ranges");
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>("QuantityX", "Calculated observable to collect for x axis",
                                                                     xObservable_, this, ProcedureNode::NodeClass::Calculate,
                                                                     true);
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>("QuantityY", "Calculated observable to collect for y axis",
                                                                     yObservable_, this, ProcedureNode::NodeClass::Calculate,
                                                                     true);
    keywords_.add<Vec3DoubleKeyword>("RangeX", "Range and binwidth of the x-axis of the histogram", rangeX_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);
    keywords_.add<Vec3DoubleKeyword>("RangeY", "Range and binwidth of the y-axis of the histogram", rangeY_,
                                     Vec3<double>(0.0, 0.0, 1.0e-5), std::nullopt, Vec3Labels::MinMaxBinwidthlabels);

    keywords_.addHidden<NodeBranchKeyword>("SubCollect", "Branch which runs if the target quantities were binned successfully",
                                           subCollectBranch_);
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

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> Collect2DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Collect2DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), procedureContext.configuration()->niceName());
    auto [target, status] = procedureContext.processingModuleData().realiseIf<Histogram2D>(
        dataName, procedureContext.processingModuleDataPrefix(), GenericItem::InRestartFileFlag);
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

    // Check target observables
    if (!xObservable_.first)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());
    if (!yObservable_.first)
        return Messenger::error("No valid y quantity set in '{}'.\n", name());

    // Prepare any branches
    if (!subCollectBranch_.prepare(procedureContext))
        return false;

    return true;
}

// Execute node
bool Collect2DProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto [xObs, xIndex] = xObservable_;
    auto [yObs, yIndex] = yObservable_;

    assert(xObs && yObs && histogram_);

    // Bin the current value of the observable
    if (histogram_->get().bin(xObs->value(xIndex), yObs->value(yIndex)))
        return subCollectBranch_.execute(procedureContext);

    return true;
}

// Finalise any necessary data after execution
bool Collect2DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    assert(histogram_);

    // Accumulate the current binned data
    histogram_->get().accumulate();

    // Finalise any branches
    if (!subCollectBranch_.finalise(procedureContext))
        return false;

    return true;
}
