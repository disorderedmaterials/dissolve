// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/sum1d.h"
#include "classes/configuration.h"
#include "io/export/data1d.h"
#include "keywords/bool.h"
#include "keywords/range.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/process1d.h"

Sum1DProcedureNode::Sum1DProcedureNode(std::shared_ptr<Process1DProcedureNode> target)
    : ProcedureNode(ProcedureNode::NodeType::Sum1D), sourceData_(target)
{
    keywords_.add<NodeKeyword<Process1DProcedureNode>>("Control", "SourceData", "Process1D node containing the data to sum",
                                                       sourceData_, this, ProcedureNode::NodeType::Process1D, false);
    keywords_.add<RangeKeyword>("Control", "RangeA", "X range for first summation region", range_[0],
                                Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<BoolKeyword>("Control", "RangeBEnabled", "Whether the second summation region is enabled", rangeEnabled_[1]);
    keywords_.add<RangeKeyword>("Control", "RangeB", "X range for second summation region", range_[1],
                                Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<BoolKeyword>("Control", "RangeCEnabled", "Whether the second summation region is enabled", rangeEnabled_[2]);
    keywords_.add<RangeKeyword>("Control", "RangeC", "X range for third summation region", range_[2],
                                Vec3Labels::MinMaxDeltaLabels);
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

// Return reference to specified range for calculation
Range &Sum1DProcedureNode::range(int n)
{
    assert(n >= 0 && n < 3);
    return range_[n];
}

// Return reference to range enabled flag
bool &Sum1DProcedureNode::rangeEnabled(int n)
{
    assert(n >= 1 && n < 3);
    return rangeEnabled_[n];
}

// Return whether range B is enabled (from keyword data)
bool Sum1DProcedureNode::isRangeBEnabled() const { return rangeEnabled_[1]; }

// Return whether range C is enabled (from keyword data)
bool Sum1DProcedureNode::isRangeCEnabled() const { return rangeEnabled_[2]; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Sum1DProcedureNode::prepare(const ProcedureContext &procedureContext)
{
    if (!sourceData_)
        return Messenger::error("No source Process1D node set in '{}'.\n", name());

    return true;
}

// Finalise any necessary data after execution
bool Sum1DProcedureNode::finalise(const ProcedureContext &procedureContext)
{
    // Calculate integrals
    const std::vector<std::string> rangeNames = {"A", "B", "C"};
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
        {
            if (!sum_[i].has_value())
                sum_[i] = procedureContext.dataList().realise<SampledDouble>(
                    fmt::format("Sum1D//{}//{}", name(), rangeNames[i]), procedureContext.dataPrefix(),
                    GenericItem::InRestartFileFlag);
            sum_[i]->get() += Integrator::sum(sourceData_->processedData(), range_[i]);
        }

    // Print info
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
            Messenger::print("Sum1D - Range {}: {:e} +/- {:e} over {:e} < x < {:e}.\n", rangeNames[i], sum_[i]->get().value(),
                             sum_[i]->get().stDev(), range_[i].minimum(), range_[i].maximum());

    return true;
}
