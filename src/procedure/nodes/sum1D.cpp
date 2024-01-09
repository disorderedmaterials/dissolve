// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "procedure/nodes/sum1D.h"
#include "classes/configuration.h"
#include "io/export/data1D.h"
#include "keywords/bool.h"
#include "keywords/range.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/operateBase.h"
#include "procedure/nodes/process1D.h"

Sum1DProcedureNode::Sum1DProcedureNode(std::shared_ptr<Process1DProcedureNode> target)
    : ProcedureNode(ProcedureNode::NodeType::Sum1D, {ProcedureNode::AnalysisContext}), sourceData_(target)
{
    keywords_.setOrganisation("Options", "Source");
    keywords_.add<NodeKeyword<Process1DProcedureNode>>("SourceData", "Process1D node containing the data to sum", sourceData_,
                                                       this, ProcedureNode::NodeType::Process1D, false);
    keywords_.add<BoolKeyword>("Instantaneous", "Calculate instantaneous sums rather than averages", instantaneous_);

    keywords_.setOrganisation("Options", "Ranges");
    keywords_.add<RangeKeyword>("RangeA", "X range for first summation region", range_[0], Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<BoolKeyword>("RangeBEnabled", "Whether the second summation region is enabled", rangeEnabled_[1]);
    keywords_.add<RangeKeyword>("RangeB", "X range for second summation region", range_[1], Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<BoolKeyword>("RangeCEnabled", "Whether the second summation region is enabled", rangeEnabled_[2]);
    keywords_.add<RangeKeyword>("RangeC", "X range for third summation region", range_[2], Vec3Labels::MinMaxDeltaLabels);
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
    assert(n >= 0 && n < 3);
    return rangeEnabled_[n];
}

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
                sum_[i] = procedureContext.processingModuleData().realise<SampledDouble>(
                    fmt::format("Sum1D//{}//{}", name(), rangeNames[i]), procedureContext.processingModuleDataPrefix(),
                    GenericItem::InRestartFileFlag);
            if (instantaneous_)
                sum_[i]->get() = Integrator::sum(sourceData_->processedData(), range_[i]);
            else
                sum_[i]->get() += Integrator::sum(sourceData_->processedData(), range_[i]);
        }

    // Print info
    for (int i = 0; i < 3; ++i)
        if (rangeEnabled_[i])
            Messenger::print("Sum1D - Range {}: {:e} +/- {:e} over {:e} < x < {:e}.\n", rangeNames[i], sum_[i]->get().value(),
                             sum_[i]->get().stDev(), range_[i].minimum(), range_[i].maximum());

    return true;
}
