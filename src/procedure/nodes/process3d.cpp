// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/process3d.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "keywords/fileandformat.h"
#include "keywords/nodebranch.h"
#include "keywords/stdstring.h"
#include "procedure/nodes/collect3d.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/select.h"

Process3DProcedureNode::Process3DProcedureNode(std::shared_ptr<Collect3DProcedureNode> target)
    : ProcedureNode(ProcedureNode::NodeType::Process3D), sourceData_(target)
{
    keywords_.add<NodeKeyword<Collect3DProcedureNode>>("Control", "SourceData",
                                                       "Collect2D node containing the histogram data to process", sourceData_,
                                                       this, ProcedureNode::NodeType::Collect3D, false);
    keywords_.add<StringKeyword>("Control", "LabelValue", "Label for the value axis", labelValue_);
    keywords_.add<StringKeyword>("Control", "LabelX", "Label for the x axis", labelX_);
    keywords_.add<StringKeyword>("Control", "LabelY", "Label for the y axis", labelY_);
    keywords_.add<StringKeyword>("Control", "LabelZ", "Label for the z axis", labelZ_);
    keywords_.add<FileAndFormatKeyword>("Export", "Export", "File format and file name under which to save processed data",
                                        exportFileAndFormat_, "EndExport");
    keywords_.addKeyword<NodeBranchKeyword>("Normalisation", "Branch providing normalisation operations for the data",
                                            normalisationBranch_, this, ProcedureNode::OperateContext);

    // Initialise branch
    normalisationBranch_ = nullptr;

    // Initialise data pointer
    processedData_ = nullptr;
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Process3DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
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

// Add and return normalisation sequence branch
std::shared_ptr<SequenceProcedureNode> Process3DProcedureNode::addNormalisationBranch()
{
    if (!normalisationBranch_)
        normalisationBranch_ =
            std::make_shared<SequenceProcedureNode>(ProcedureNode::OperateContext, shared_from_this(), "Normalisation");

    return normalisationBranch_;
}

// Return whether this node has a branch
bool Process3DProcedureNode::hasBranch() const { return (normalisationBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
std::shared_ptr<SequenceProcedureNode> Process3DProcedureNode::branch() { return normalisationBranch_; }

// Find the nodes owned by this node
std::vector<ConstNodeRef> Process3DProcedureNode::children() const { return {normalisationBranch_}; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process3DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!sourceData_)
        return Messenger::error("No source Collect3D node set in '{}'.\n", name());

    if (normalisationBranch_)
        normalisationBranch_->prepare(procedureContext);

    return true;
}

// Finalise any necessary data after execution
bool Process3DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = procedureContext.dataList().realise<Data3D>(fmt::format("Process3D//{}", name()),
                                                             procedureContext.dataPrefix(), GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process3D node
    data = sourceData_->accumulatedData();

    // Run normalisation on the data
    if (normalisationBranch_)
    {
        // Set data targets in the normalisation nodes  TODO Will not work for sub-branches, if they are ever required
        for (auto node : normalisationBranch_->sequence())
        {
            if (node->nodeClass() != ProcedureNode::NodeClass::Operate)
                continue;

            // Cast the node
            auto operateNode = std::dynamic_pointer_cast<OperateProcedureNodeBase>(node);
            operateNode->setTarget(processedData_);
        }

        if (!normalisationBranch_->execute(procedureContext))
            return false;
    }

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
