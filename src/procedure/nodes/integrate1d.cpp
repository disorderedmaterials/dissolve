// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/nodes/integrate1d.h"
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

Integrate1DProcedureNode::Integrate1DProcedureNode(const Process1DProcedureNode *target)
    : ProcedureNode(ProcedureNode::NodeType::Integrate1D)
{
    keywords_.add("Control",
                  new NodeKeyword<const Process1DProcedureNode>(this, ProcedureNode::NodeType::Process1D, false, target),
                  "SourceData", "Process1D node containing the data to integrate");
    keywords_.add("Control", new RangeKeyword(Range(0.0, 3.0), Vec3Labels::MinMaxDeltaLabels), "RangeA",
                  "X range for first integration region");
    keywords_.add("Control", new RangeKeyword(Range(3.0, 6.0), Vec3Labels::MinMaxDeltaLabels), "RangeB",
                  "X range for second integration region");
    keywords_.add("Control", new RangeKeyword(Range(6.0, 9.0), Vec3Labels::MinMaxDeltaLabels), "RangeC",
                  "X range for third integration region");
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
    // Retrieve the Process1D node target
    processNode_ = keywords_.retrieve<const Process1DProcedureNode *>("SourceData");
    if (!processNode_)
        return Messenger::error("No source Process1D node set in '{}'.\n", name());

    return true;
}

// Finalise any necessary data after execution
bool Integrate1DProcedureNode::finalise(ProcessPool &procPool, Configuration *cfg, std::string_view prefix,
                                        GenericList &targetList)
{
    // Get ranges
    auto rangeA = keywords_.retrieve<Range>("RangeA");
    auto rangeB = keywords_.retrieve<Range>("RangeB");
    auto rangeC = keywords_.retrieve<Range>("RangeC");

    // Calculate integrals
    integral_[0] += Integrator::trapezoid(processNode_->processedData(), keywords_.retrieve<Range>("RangeA"));
    integral_[1] += Integrator::trapezoid(processNode_->processedData(), keywords_.retrieve<Range>("RangeB"));
    integral_[2] += Integrator::trapezoid(processNode_->processedData(), keywords_.retrieve<Range>("RangeC"));

    // Print info
    Messenger::print("Integrate1D - Range A: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[0].value(), integral_[0].stDev(),
                     rangeA.minimum(), rangeA.maximum());
    Messenger::print("Integrate1D - Range B: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[1].value(), integral_[1].stDev(),
                     rangeB.minimum(), rangeB.maximum());
    Messenger::print("Integrate1D - Range C: {:e} +/- {:e} over {:e} < x < {:e}.\n", integral_[2].value(), integral_[2].stDev(),
                     rangeC.minimum(), rangeC.maximum());

    return true;
}
