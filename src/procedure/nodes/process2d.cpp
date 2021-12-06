// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/process2d.h"
#include "classes/configuration.h"
#include "io/export/data2d.h"
#include "keywords/fileandformat.h"
#include "keywords/nodebranch.h"
#include "keywords/stdstring.h"
#include "math/integrator.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/operatebase.h"

Process2DProcedureNode::Process2DProcedureNode(std::shared_ptr<Collect2DProcedureNode> target)
    : ProcedureNode(ProcedureNode::NodeType::Process2D), sourceData_(target)
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

    // Initialise branch
    normalisationBranch_ = nullptr;

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

// Add and return subcollection sequence branch
std::shared_ptr<SequenceProcedureNode> Process2DProcedureNode::addNormalisationBranch()
{
    if (!normalisationBranch_)
        normalisationBranch_ = std::make_shared<SequenceProcedureNode>(ProcedureNode::OperateContext, procedure());

    return normalisationBranch_;
}

// Return whether this node has a branch
bool Process2DProcedureNode::hasBranch() const { return (normalisationBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
std::shared_ptr<SequenceProcedureNode> Process2DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process2DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    if (!sourceData_)
        return Messenger::error("No source Collect2D node set in '{}'.\n", name());

    if (normalisationBranch_)
        normalisationBranch_->prepare(cfg, prefix, targetList);

    return true;
}

// Finalise any necessary data after execution
bool Process2DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = targetList.realise<Data2D>(fmt::format("Process2D//{}", name()), prefix, GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process1D node
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

        if (!normalisationBranch_->execute(procPool, cfg, prefix, targetList))
            return false;
    }

    // Save data?
    if (exportFileAndFormat_.hasFilename())
    {
        if (procPool.isMaster())
        {
            if (exportFileAndFormat_.exportData(data))
                procPool.decideTrue();
            else
            {
                procPool.decideFalse();
                return false;
            }
        }
        else if (!procPool.decision())
            return false;
    }

    return true;
}
