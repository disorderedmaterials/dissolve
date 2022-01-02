// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "procedure/nodes/integrate1d.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "io/export/data1d.h"
#include "keywords/range.h"
#include "math/integrator.h"
#include "modules/analyse/analyse.h"
#include "procedure/nodes/operatebase.h"
#include "procedure/nodes/process1d.h"

Integrate1DProcedureNode::Integrate1DProcedureNode(std::shared_ptr<Process1DProcedureNode> target)
    : ProcedureNode(ProcedureNode::NodeType::Integrate1D), sourceData_(target)
{
    keywords_.add<NodeKeyword<Process1DProcedureNode>>("Control", "SourceData",
                                                       "Process1D node containing the data to integrate", sourceData_, this,
                                                       ProcedureNode::NodeType::Process1D, false);
    keywords_.add<RangeKeyword>("Control", "RangeA", "X range for first integration region", range_[0],
                                Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<RangeKeyword>("Control", "RangeB", "X range for second integration region", range_[1],
                                Vec3Labels::MinMaxDeltaLabels);
    keywords_.add<RangeKeyword>("Control", "RangeC", "X range for third integration region", range_[2],
                                Vec3Labels::MinMaxDeltaLabels);
}

/*
 * Identity
 */

// Return whether specified context is relevant for this node type
bool Integrate1DProcedureNode::isContextRelevant(ProcedureNode::NodeContext context)
{
    return (context == ProcedureNode::AnalysisContext);
}

/*
 * Data
 */

// Return calculated integral
const SampledDouble &Integrate1DProcedureNode::integral(int index) const { return integral_[index]; }

/*
 * Execute
 */

// Prepare any necessary data, ready for execution
bool Integrate1DProcedureNode::prepare(Configuration *cfg, std::string_view prefix, GenericList &targetList)
{
    if (!sourceData_)
        return Messenger::error("No source Process1D node set in '{}'.\n", name());

    return true;
}

// Finalise any necessary data after execution
bool Integrate1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                        GenericList &targetList)
{
    // Calculate integrals
    integral_[0] += Integrator::trapezoid(sourceData_->processedData(), range_[0]);
    integral_[1] += Integrator::trapezoid(sourceData_->processedData(), range_[1]);
    integral_[2] += Integrator::trapezoid(sourceData_->processedData(), range_[2]);

    // Print info
    Messenger::print("Integrate1D - Range A: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[0].value(), integral_[0].stDev(),
                     range_[0].minimum(), range_[0].maximum());
    Messenger::print("Integrate1D - Range B: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[1].value(), integral_[1].stDev(),
                     range_[1].minimum(), range_[1].maximum());
    Messenger::print("Integrate1D - Range C: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[2].value(), integral_[2].stDev(),
                     range_[2].minimum(), range_[2].maximum());

    return true;
}
