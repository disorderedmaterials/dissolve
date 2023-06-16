// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "procedure/nodes/process1d.h"
#include "classes/configuration.h"
#include "io/export/data1d.h"
#include "keywords/bool.h"
#include "keywords/fileandformat.h"
#include "keywords/nodebranch.h"
#include "keywords/stdstring.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/collect1d.h"
#include "procedure/nodes/operatebase.h"

Process1DProcedureNode::Process1DProcedureNode(std::shared_ptr<Collect1DProcedureNode> target,
                                               ProcedureNode::NodeContext normalisationContext)
    : ProcedureNode(ProcedureNode::NodeType::Process1D), sourceData_(target),
      normalisationBranch_(normalisationContext, *this, "Normalisation")
{
    keywords_.setOrganisation("Options", "Source");
    keywords_.add<NodeKeyword<Collect1DProcedureNode>>("SourceData", "Collect1D node containing the histogram data to process",
                                                       sourceData_, this, ProcedureNode::NodeType::Collect1D, false);
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

// Return the branch from this node (if it has one)
OptionalReferenceWrapper<ProcedureNodeSequence> Process1DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process1DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!sourceData_)
        return Messenger::error("No source Collect1D node set in '{}'.\n", name());

    normalisationBranch_.prepare(procedureContext);

    return true;
}

// Finalise any necessary data after execution
bool Process1DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Retrieve / realise the normalised data from the supplied list
    auto &data = procedureContext.dataList().realise<Data1D>(fmt::format("Process1D//{}", name()),
                                                             procedureContext.dataPrefix(), GenericItem::InRestartFileFlag);
    processedData_ = &data;
    data.setTag(name());

    // Copy the averaged data from the associated Process1D node
    if (instantaneous_)
        data = sourceData_->data();
    else
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
