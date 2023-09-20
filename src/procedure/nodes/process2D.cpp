// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/process2D.h"
#include "classes/configuration.h"
#include "io/export/data2D.h"
#include "keywords/fileAndFormat.h"
#include "keywords/nodeBranch.h"
#include "keywords/stdString.h"
#include "math/integrator.h"
#include "procedure/nodes/collect2D.h"
#include "procedure/nodes/operateBase.h"

Process2DProcedureNode::Process2DProcedureNode(std::shared_ptr<Collect2DProcedureNode> target,
                                               ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process2D, {ProcedureNode::AnalysisContext}), sourceData_(target),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    keywords_.setOrganisation("Options", "Control");
    keywords_.add<NodeKeyword<Collect2DProcedureNode>>("SourceData", "Collect2D node containing the histogram data to process",
                                                       sourceData_, this, ProcedureNode::NodeType::Collect2D, false);
    keywords_.setOrganisation("Options", "Labels");
    keywords_.add<StringKeyword>("LabelValue", "Label for the value axis", labelValue_);
    keywords_.add<StringKeyword>("LabelX", "Label for the x axis", labelX_);
    keywords_.add<StringKeyword>("LabelY", "Label for the y axis", labelY_);

    keywords_.setOrganisation("Options", "Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save processed data",
                                        exportFileAndFormat_, "EndExport");

    keywords_.addHidden<NodeBranchKeyword>("Normalisation", "Branch providing normalisation operations for the data",
                                           normalisationBranch_);

    // Initialise data pointer
    processedData_ = nullptr;
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
    auto &data = procedureContext.processingModuleData().realise<Data2D>(
        fmt::format("Process2D//{}", name()), procedureContext.processingModuleDataPrefix(), GenericItem::InRestartFileFlag);
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
