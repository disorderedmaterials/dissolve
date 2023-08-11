// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/iteratedata1d.h"
#include "classes/configuration.h"
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
                                                       ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process1D, {ProcedureNode::AnalysisContext}), sourceData_(target),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    setUpKeywords();
}

IterateData1DProcedureNode::IterateData1DProcedureNode(std::shared_ptr<IntegerCollect1DProcedureNode> intTarget,
                                                       ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process1D, {ProcedureNode::AnalysisContext}), sourceIntegerData_(intTarget),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
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
}

/*
 * Data
 */

// Return whether processed data exists
bool IterateData1DProcedureNode::hasProcessedData() const { return (processedData_ != nullptr); }

// Return processed data
const Data1D &IterateData1DProcedureNode::processedData() const
{
    if (!processedData_)
    {
        Messenger::error("No processed data pointer set in IterateDat1DProcedureNode, so nothing to return.\n");
        static Data1D dummy;
        return dummy;
    }

    return (*processedData_);
}
/*
 * Branches
 */

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
        normalisationBranch_.prepare(procedureContext);

        return true;
    }
    else
        return Messenger::error("No source data node set in '{}'.\n", name());
}

// Finalise any necessary data after execution
bool IterateData1DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = procedureContext.dataList().realise<Data1D>(
        fmt::format("Process1D//{}", name()), procedureContext.processingDataPrefix(), GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process1D node
    if (instantaneous_)
        data = sourceData_ ? sourceData_->data() : sourceIntegerData_->data();
    else
        data = sourceData_ ? sourceData_->accumulatedData() : sourceIntegerData_->accumulatedData();

    // Run normalisation on the data
    // Set data targets in the normalisation nodes
    for (auto &node : normalisationBranch_.sequence())
    {
        if (node->nodeClass() != ProcedureNode::NodeClass::Operate)
            continue;

        // Cast the node
        auto operateNode = std::dynamic_pointer_cast<OperateProcedureNodeBase>(node);
        operateNode->setTarget(processedData_);
    }

    if (!normalisationBranch_.execute(procedureContext))
        return false;

    return true;
}
