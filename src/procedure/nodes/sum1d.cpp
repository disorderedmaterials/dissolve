// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/sum1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data1d.h"
#include "keywords/types.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/select.h"

Sum1DProcedureNode::Sum1DProcedureNode(Process1DProcedureNode *target) : ProcedureNode(ProcedureNode::NodeType::Sum1D)
{
    keywords_.add("Control", new NodeKeyword(this, ProcedureNode::NodeType::Process1D, false, target), "SourceData",
                  "Process1D node containing the data to sum");
    keywords_.add("Control", new RangeKeyword(Range(0.0, 3.0), Vec3Labels::MinMaxDeltaLabels), "RangeA",
                  "X range for first summation region");
    keywords_.add("Control", new BoolKeyword(false), "RangeBEnabled", "Whether the second summation region is enabled");
    keywords_.add("Control", new RangeKeyword(Range(3.0, 6.0), Vec3Labels::MinMaxDeltaLabels), "RangeB",
                  "X range for second summation region");
    keywords_.add("Control", new BoolKeyword(false), "RangeCEnabled", "Whether the second summation region is enabled");
    keywords_.add("Control", new RangeKeyword(Range(6.0, 9.0), Vec3Labels::MinMaxDeltaLabels), "RangeC",
                  "X range for third summation region");
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Sum1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return calculated sum
SampledDouble Sum1DProcedureNode::sum(int index) const
{
    if (sum_[index].has_value())
        return sum_[index].value();
    return {};
}

// Return whether range B is enabled (from keyword data)
bool Sum1DProcedureNode::isRangeBEnabled() const { return keywords_.asBool("RangeBEnabled"); }

// Return whether range C is enabled (from keyword data)
bool Sum1DProcedureNode::isRangeCEnabled() const { return keywords_.asBool("RangeCEnabled"); }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Sum1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Retrieve the Process1D node target
    processNode_ = dynamic_cast<const Process1DProcedureNode *>(keywords_.retrieve<const ProcedureNode *>("SourceData"));
    if (!processNode_)
        return Messenger::error("No source Process1D node set in '{}'.\n", name());

    // Get ranges and status flags
    range_[0] = keywords_.retrieve<Range>("RangeA");
    range_[1] = keywords_.retrieve<Range>("RangeB");
    range_[2] = keywords_.retrieve<Range>("RangeC");
    rangeEnabled_[0] = true;
    rangeEnabled_[1] = keywords_.asBool("RangeBEnabled");
    rangeEnabled_[2] = keywords_.asBool("RangeCEnabled");

    return true;
}

// Finalise any necessary data after execution
bool Sum1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    // Calculate integrals
    std::vector<std::string> rangeNames = {"A", "B", "C"}
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
            if (!sum_[i].has_value())
            {
                sum_[i] =
                    targetList.realise<SampledDouble>(fmt::format("Sum1D//{}//{}", name(), rangeNames[i]), prefix, GenericItem::InRestartFileFlag);
                sum_[i]->get() += Integrator::sum(processNode_->processedData(), range_[i]);
            }

    // Print info
    for (int i=0; i < 3; ++i)
        if (rangeEnabled_[i])
            Messenger::print("Sum1D - Range {}: {:e} +/- {:e} over {:e} < x < {:e}.\n", rangeNames[i], sum_[i]->get().value(),
                         sum_[i]->get().stDev(), range_[i].minimum(), range_[i].maximum());

    return true;
}
