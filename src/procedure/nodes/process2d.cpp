// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/process2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data2d.h"
#include "keywords/types.h"
#include "math/integrator.h"
#include "procedure/nodes/collect2d.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/select.h"

Process2DProcedureNode::Process2DProcedureNode(const Collect2DProcedureNode *target)
    : ProcedureNode(ProcedureNode::NodeType::Process2D)
{
    keywords_.add("Control",
                  new NodeKeyword<const Collect2DProcedureNode>(this, ProcedureNode::NodeType::Collect2D, false, target),
                  "SourceData", "Collect2D node containing the histogram data to process");
    keywords_.add("Control", new StringKeyword("Counts"), "LabelValue", "Label for the value axis");
    keywords_.add("Control", new StringKeyword("X"), "LabelX", "Label for the x axis");
    keywords_.add("Control", new StringKeyword("Y"), "LabelY", "Label for the y axis");
    keywords_.add("Export", new BoolKeyword(false), "Save", "Save processed data to disk");
    keywords_.add("HIDDEN", new NodeBranchKeyword(this, &normalisationBranch_, ProcedureNode::OperateContext), "Normalisation",
                  "Branch providing normalisation operations for the data");

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

// Return value label
std::string Process2DProcedureNode::valueLabel() const { return keywords_.asString("LabelValue"); }

// Return x axis label
std::string Process2DProcedureNode::xAxisLabel() const { return keywords_.asString("LabelX"); }

// Return y axis label
std::string Process2DProcedureNode::yAxisLabel() const { return keywords_.asString("LabelY"); }

/*
 * Branches
 */

// Add and return subcollection sequence branch
SequenceProcedureNode *Process2DProcedureNode::addNormalisationBranch()
{
    if (!normalisationBranch_)
        normalisationBranch_ = new SequenceProcedureNode(ProcedureNode::OperateContext, procedure());

    return normalisationBranch_;
}

// Return whether this node has a branch
bool Process2DProcedureNode::hasBranch() const { return (normalisationBranch_ != nullptr); }

// Return SequenceNode for the branch (if it exists)
SequenceProcedureNode *Process2DProcedureNode::branch() { return normalisationBranch_; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Process2DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve the Collect2D node target
    collectNode_ = keywords_.retrieve<const Collect2DProcedureNode *>("SourceData");
    if (!collectNode_)
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
    data = collectNode_->accumulatedData();

    // Run normalisation on the data
    if (normalisationBranch_)
    {
        // Set data targets in the normalisation nodes  TODO Will not work for sub-branches, if they are ever required
        ListIterator<ProcedureNode> nodeIterator(normalisationBranch_->sequence());
        while (ProcedureNode *node = nodeIterator.iterate())
        {
            if (!node->isType(ProcedureNode::NodeType::OperateBase))
                continue;

            // Cast the node
            auto *operateNode = dynamic_cast<OperateProcedureNodeBase *>(node);
            operateNode->setTarget(processedData_);
        }

        if (!normalisationBranch_->execute(procPool, cfg, prefix, targetList))
            return false;
    }

    // Save data?
    if (keywords_.asBool("Save"))
    {
        if (procPool.isMaster())
        {
            Data2DExportFileFormat exportFormat(fmt::format("{}_{}.txt", name(), cfg->name()));
            if (exportFormat.exportData(data))
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
