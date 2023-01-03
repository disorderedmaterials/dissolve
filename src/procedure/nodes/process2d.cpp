// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/process2d.h"
#include "classes/configuration.h"
#include "io/export/data2d.h"
#include "keywords/fileandformat.h"
#include "keywords/nodebranch.h"
#include "keywords/stdstring.h"
#include "math/integrator.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/operatebase.h"

Process2DProcedureNode::Process2DProcedureNode(std::shared_ptr<Collect2DProcedureNode> target,
                                               ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process2D), sourceData_(target),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    keywords_.add<NodeKeyword<Collect2DProcedureNode>>("Control", "SourceData",
                                                       "Collect2D node containing the histogram data to process", sourceData_,
                                                       this, ProcedureNode::NodeType::Collect2D, false);
    keywords_.add<StringKeyword>("Control", "LabelValue", "Label for the value axis", labelValue_);
    keywords_.add<StringKeyword>("Control", "LabelX", "Label for the x axis", labelX_);
    keywords_.add<StringKeyword>("Control", "LabelY", "Label for the y axis", labelY_);
    keywords_.add<FileAndFormatKeyword>("Export", "Export", "File format and file name under which to save processed data",
                                        exportFileAndFormat_, "EndExport");
    keywords_.addKeyword<NodeBranchKeyword>("Normalisation", "Branch providing normalisation operations for the data",
                                            normalisationBranch_, this, ProcedureNode::OperateContext);

    // Initialise data pointer
    processedData_ = nullptr;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Process2DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return processed data
const Data2D &Process2DProcedureNode::processedData() const
{
    if (!processedData_)
    {
        Messenger::error("No processed data pointer set in Process2DProcedureNode, so nothing to return.\n");
        static Data2D dummy;
        return dummy;
    }

    return (*processedData_);
}

// Return export file and format for processed data
Data2DExportFileFormat &Process2DProcedureNode::exportFileAndFormat() { return exportFileAndFormat_; }

// Return value label
std::string Process2DProcedureNode::valueLabel() const { return labelValue_; }

// Return x axis label
std::string Process2DProcedureNode::xAxisLabel() const { return labelX_; }

// Return y axis label
std::string Process2DProcedureNode::yAxisLabel() const { return labelY_; }

/*
 * Branches
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> Process2DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process2DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!sourceData_)
        return Messenger::error("No source Collect2D node set in '{}'.\n", name());

    normalisationBranch_.prepare(procedureContext);

    return true;
}

// Finalise any necessary data after execution
bool Process2DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = procedureContext.dataList().realise<Data2D>(fmt::format("Process2D//{}", name()),
                                                             procedureContext.dataPrefix(), GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process1D node
    data = sourceData_->accumulatedData();

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

    // Save data?
    if (exportFileAndFormat_.hasFilename())
    {
        if (procedureContext.processPool().isMaster())
        {
            if (exportFileAndFormat_.exportData(data))
                procedureContext.processPool().decideTrue();
            else
            {
                procedureContext.processPool().decideFalse();
                return false;
            }
        }
        else if (!procedureContext.processPool().decision())
            return false;
    }

    return true;
}
