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
bool IterateData1DProcedureNode::prepare(const ProcedureContext &procedureContext) { return true; }

// Finalise any necessary data after execution
bool IterateData1DProcedureNode::finalise(const ProcedureContext &procedureContext) { return true; }
