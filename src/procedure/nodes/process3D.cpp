// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/process3D.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/nodeBranch.h"
#include "keywords/stdString.h"
#include "procedure/nodes/collect3D.h"
#include "procedure/nodes/operateBase.h"
#include "procedure/nodes/select.h"

Process3DProcedureNode::Process3DProcedureNode(std::shared_ptr<Collect3DProcedureNode> target,
                                               ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process3D, {ProcedureNode::AnalysisContext}), sourceData_(target),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    keywords_.setOrganisation("Options", "Control");
    keywords_.add<NodeKeyword<Collect3DProcedureNode>>("SourceData", "Collect2D node containing the histogram data to process",
                                                       sourceData_, this, ProcedureNode::NodeType::Collect3D, false);

    keywords_.setOrganisation("Options", "Labels");
    keywords_.add<StringKeyword>("LabelValue", "Label for the value axis", labelValue_);
    keywords_.add<StringKeyword>("LabelX", "Label for the x axis", labelX_);
    keywords_.add<StringKeyword>("LabelY", "Label for the y axis", labelY_);
    keywords_.add<StringKeyword>("LabelZ", "Label for the z axis", labelZ_);

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
const Data3D &Process3DProcedureNode::processedData() const
{
    if (!processedData_)
    {
        Messenger::error("No processed data pointer set in Process3DProcedureNode, so nothing to return.\n");
        static Data3D dummy;
        return dummy;
    }

    return (*processedData_);
}

// Return export file and format for processed data
Data3DExportFileFormat &Process3DProcedureNode::exportFileAndFormat() { return exportFileAndFormat_; }

// Return value label
std::string Process3DProcedureNode::valueLabel() const { return labelValue_; }

// Return x axis label
std::string Process3DProcedureNode::xAxisLabel() const { return labelX_; }

// Return y axis label
std::string Process3DProcedureNode::yAxisLabel() const { return labelY_; }

// Return z axis label
std::string Process3DProcedureNode::zAxisLabel() const { return labelZ_; }

/*
 * Branches
 */

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> Process3DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process3DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!sourceData_)
        return Messenger::error("No source Collect3D node set in '{}'.\n", name());

    normalisationBranch_.prepare(procedureContext);

    return true;
}

// Finalise any necessary data after execution
bool Process3DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = procedureContext.processingModuleData().realise<Data3D>(
        fmt::format("Process3D//{}", name()), procedureContext.processingModuleDataPrefix(), GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process3D node
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
