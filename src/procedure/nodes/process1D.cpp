// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/process1D.h"
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

Process1DProcedureNode::Process1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target,
                                               ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process1D, {ProcedureNode::AnalysisContext}), sourceData_(target),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    setUpKeywords();
}

Process1DProcedureNode::Process1DProcedureNode(std::shared_ptr<IntegerCollect1DProcedureNode> intTarget,
                                               ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process1D, {ProcedureNode::AnalysisContext}), sourceIntegerData_(intTarget),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    setUpKeywords();
}

// Set up keywords for node
void Process1DProcedureNode::setUpKeywords()
{
    keywords_.setOrganisation("Options", "Source");
    keywords_.add<NodeKeyword<Collect1DProcedureNode>>("SourceData", "Collect1D node containing the histogram data to process",
                                                       sourceData_, this, ProcedureNode::NodeType::Collect1D, false);
    keywords_.add<NodeKeyword<IntegerCollect1DProcedureNode>>(
        "SourceIntegerData", "IntegerCollect1D node containing the histogram data to process", sourceIntegerData_, this,
        ProcedureNode::NodeType::IntegerCollect1D, false);
    keywords_.add<BoolKeyword>(
        "Instantaneous", "Whether to use only the current binned data of the histogram, rather than the accumulated average",
        instantaneous_);

    keywords_.setOrganisation("Options", "Labels");
    keywords_.add<StringKeyword>("LabelValue", "Label for the value axis", labelValue_);
    keywords_.add<StringKeyword>("LabelX", "Label for the x axis", labelX_);

    keywords_.setOrganisation("Options", "Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save processed data",
                                        exportFileAndFormat_, "EndExport");

    keywords_.addHidden<NodeBranchKeyword>("Normalisation", "Branch providing normalisation operations for the data",
                                           normalisationBranch_);
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

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> Process1DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process1DProcedureNode::prepare(const ProcedureContext &procedureContext)
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
bool Process1DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = procedureContext.processingModuleData().realise<Data1D>(
        fmt::format("Process1D//{}", name()), procedureContext.processingModuleDataPrefix(), GenericItem::InRestartFileFlag);
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
