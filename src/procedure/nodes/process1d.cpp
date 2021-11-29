// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/process1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data1d.h"
#include "keywords/types.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/select.h"

Process1DProcedureNode::Process1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target)
    : ProcedureNode(ProcedureNode::NodeType::Process1D), sourceData_(target)
{
    keywords_.add<NodeKeyword<Collect1DProcedureNode>>("Control", "SourceData",
                                                       "Collect1D node containing the histogram data to process", sourceData_,
                                                       this, ProcedureNode::NodeType::Collect1D, false);
    keywords_.add<BoolKeyword>(
        "Control", "CurrentDataOnly",
        "Whether to use only the current binned data of the histogram, rather than the accumulated average", currentDataOnly_);
    keywords_.add<StringKeyword>("Control", "LabelValue", "Label for the value axis", labelValue_);
    keywords_.add<StringKeyword>("Control", "LabelX", "Label for the x axis", labelX_);
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
bool Process1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return whether processed data exists
bool Process1DProcedureNode::hasProcessedData() const { return (processedData_ != nullptr); }

// Return processed data
const Data1D &Process1DProcedureNode::processedData() const
{
    if (!processedData_)
    {
        Messenger::error("No processed data pointer set in Process1DProcedureNode, so nothing to return.\n");
        static Data1D dummy;
        return dummy;
    }

    return (*processedData_);
}

// Return export file and format for processed data
Data1DExportFileFormat &Process1DProcedureNode::exportFileAndFormat() { return exportFileAndFormat_; }

// Return value label
std::string Process1DProcedureNode::valueLabel() const { return labelValue_; }

// Return x axis label
std::string Process1DProcedureNode::xAxisLabel() const { return labelX_; }

/*
 * Branches
 */

// Add and return subcollection sequence branch
std::shared_ptr<SequenceProcedureNode> Process1DProcedureNode::addNormalisationBranch()
{
    if (!normalisationBranch_)
        normalisationBranch_ = std::make_shared<SequenceProcedureNode>(ProcedureNode::OperateContext, procedure());

    return normalisationBranch_;
}

// Return whether this node has a branch
bool Process1DProcedureNode::hasBranch() const { return (normalisationBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
std::shared_ptr<SequenceProcedureNode> Process1DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    if (!sourceData_)
        return Messenger::error("No source Collect1D node set in '{}'.\n", name());

    if (normalisationBranch_)
        normalisationBranch_->prepare(cfg, prefix, targetList);

    return true;
}

// Finalise any necessary data after execution
bool Process1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                      GenericList &targetList)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = targetList.realise<Data1D>(fmt::format("Process1D//{}", name()), prefix, GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process1D node
    if (currentDataOnly_)
        data = sourceData_->data();
    else
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
