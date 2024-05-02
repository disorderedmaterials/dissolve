// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/integerCollect1D.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "keywords/nodeAndInteger.h"
#include "keywords/nodeBranch.h"
#include "keywords/optionalInt.h"
#include "math/data1D.h"
#include "procedure/nodes/calculateBase.h"
#include "procedure/nodes/sequence.h"

IntegerCollect1DProcedureNode::IntegerCollect1DProcedureNode(std::shared_ptr<CalculateProcedureNodeBase> observable,
                                                             ProcedureNode::NodeContext subCollectContext)
    : ProcedureNode(ProcedureNode::NodeType::IntegerCollect1D, {ProcedureNode::AnalysisContext}), xObservable_{observable, 0},
      subCollectBranch_(subCollectContext, *this, "SubCollect")
{
    keywords_.setOrganisation("Options", "Quantity / Range");
    keywords_.add<NodeAndIntegerKeyword<CalculateProcedureNodeBase>>(
        "QuantityX", "Calculated observable to collect", xObservable_, this, ProcedureNode::NodeClass::Calculate, true);
    keywords_.add<OptionalIntegerKeyword>("Minimum", "Minimum allowed bin value for the histogram", minimum_, 0, std::nullopt,
                                          1, "No Limit");
    keywords_.add<OptionalIntegerKeyword>("Maximum", "Maximum allowed bin value for the histogram", maximum_, 0, std::nullopt,
                                          1, "No Limit");
    keywords_.addHidden<NodeBranchKeyword>("SubCollect", "Branch which runs if the target quantity was binned successfully",
                                           subCollectBranch_);
}

/*
 * Data
 */

// Return current data
Data1D IntegerCollect1DProcedureNode::data() const
{
    assert(histogram_);

    return histogram_->get().data();
}

// Return accumulated data
const Data1D &IntegerCollect1DProcedureNode::accumulatedData() const
{
    assert(histogram_);

    return histogram_->get().accumulatedData();
}

/*
 * Branches
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> IntegerCollect1DProcedureNode::branch() { return subCollectBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool IntegerCollect1DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    // Construct our data name, and search for it in the supplied list
    std::string dataName = fmt::format("{}_{}_Bins", name(), procedureContext.configuration()->niceName());
    auto [target, status] = procedureContext.processingModuleData().realiseIf<IntegerHistogram1D>(
        dataName, procedureContext.processingModuleDataPrefix(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
    {
        Messenger::printVerbose("One-dimensional histogram data for '{}' was not in the target list, so it will now be "
                                "initialised...\n",
                                name());
        target.initialise(minimum_, maximum_);
    }

    // Zero the current bins, ready for the new pass
    target.zeroBins();

    // Store a reference to the data
    histogram_ = target;

    // Check target observable
    if (!xObservable_.first)
        return Messenger::error("No valid x quantity set in '{}'.\n", name());

    // Prepare any branches
    if (!subCollectBranch_.prepare(procedureContext))
        return false;

    return true;
}

// Execute node
bool IntegerCollect1DProcedureNode::execute(const ProcedureContext &procedureContext)
{
    auto [observable, index] = xObservable_;

    assert(observable && histogram_);

    // Bin the current value of the observable, and execute sub-collection branch on success
    if (histogram_->get().bin(observable->value(index)))
        return subCollectBranch_.execute(procedureContext);

    return true;
}

// Finalise any necessary data after execution
bool IntegerCollect1DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    assert(histogram_);

    // Accumulate the current binned data
    histogram_->get().accumulate();

    // Finalise any branches
    if (!subCollectBranch_.finalise(procedureContext))
        return false;

    return true;
}
