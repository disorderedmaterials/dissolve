// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/iteratedata1d.h"
#include "classes/configuration.h"
#include "expression/variable.h"
#include "io/export/data1D.h"
#include "keywords/bool.h"
#include "keywords/fileAndFormat.h"
#include "keywords/nodeBranch.h"
#include "keywords/stdString.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/integerCollect1D.h"
#include "procedure/nodes/operateBase.h"

IterateData1DProcedureNode::IterateData1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target,
                                                       ProcedureNode::NodeContext forEachContext)
    : ProcedureNode(ProcedureNode::NodeType::IterateData1D, {ProcedureNode::AnalysisContext}), sourceData_(target),
      forEachBranch_(forEachContext, *this, "ForEach")
{
    setUpKeywords();
}

IterateData1DProcedureNode::IterateData1DProcedureNode(std::shared_ptr<IntegerCollect1DProcedureNode> intTarget,
                                                       ProcedureNode::NodeContext forEachContext)
    : ProcedureNode(ProcedureNode::NodeType::IterateData1D, {ProcedureNode::AnalysisContext}), sourceIntegerData_(intTarget),
      forEachBranch_(forEachContext, *this, "ForEach")
{
    setUpKeywords();
}

// Set up keywords for node
void IterateData1DProcedureNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Source");
    keywords_.add<NodeKeyword<Collect1DProcedureNode>>("SourceData", "Collect1D node containing the histogram data to process",
                                                       sourceData_, this, ProcedureNode::NodeType::Collect1D, false);
    keywords_.add<NodeKeyword<IntegerCollect1DProcedureNode>>(
        "SourceIntegerData", "IntegerCollect1D node containing the histogram data to process", sourceIntegerData_, this,
        ProcedureNode::NodeType::IntegerCollect1D, false);
    keywords_.addHidden<NodeBranchKeyword>("ForEach", "Branch to run on each site selected", forEachBranch_);
}

/*
 * Branch
 */
// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> IterateData1DProcedureNode::branch() { return forEachBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool IterateData1DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (sourceData_ && sourceIntegerData_)
    {
        return Messenger::error("Specify either SourceData or SourceIntegerData, not both.\n");
    }
    else if (sourceData_ || sourceIntegerData_)
    {
        // If one exists, prepare the ForEach branch nodes
        if (!forEachBranch_.prepare(procedureContext))
            return false;
        else
            return true;
    }
    else
        return Messenger::error("No source data node set in '{}'.\n", name());
}

bool IterateData1DProcedureNode::execute(const ProcedureContext &procedureContext)
{
    // If a ForEach branch has been defined, process it for each of our sites in turn. Otherwise, we're done.
    if (!forEachBranch_.empty())
    {
        // Retrieve / realise the normalised data from the supplied list
        auto &data = procedureContext.dataList().realise<Data1D>(
            fmt::format("Process1D//{}", name()), procedureContext.processingDataPrefix(), GenericItem::InRestartFileFlag);
        data.setTag(name());

        // Copy the averaged data from the associated Process1D node
        data = sourceData_ ? sourceData_->data() : sourceIntegerData_->data();

        for (auto index : data.values())
        {
            xParameter_->setValue(data.xAxis(index));
            valueParameter_->setValue(data.value(index));

            // If the branch fails at any point, return failure here.  Otherwise, continue the loop
            if (!forEachBranch_.execute(procedureContext))
                return false;
        }
        return true;
    }
    return true;
}

// Finalise any necessary data after execution
bool IterateData1DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // If one exists, finalise the ForEach branch nodes
    if (!forEachBranch_.finalise(procedureContext))
        return false;
    return true;
}
